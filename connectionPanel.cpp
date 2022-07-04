/*
 * microRadar software
 * (progetto polimi 2022)
 *
 * Salvatore Passaro
 */

#include "connectionPanel.h"
#include <wx/choice.h>

/*
 * connectionPanel constructor
 * constructs connection gui pulling values from the passed reference to persistenceManager
 */
connectionPanel::connectionPanel(wxWindow *parent, persistenceManager& persistence) : wxPanel(parent) {
    json settings = persistence.getConnectionSettings();

    try {
        autoStuff = new autoStuffPanel(this, settings["autoOpenConnection"], settings["autoStartLogging"]);
        sizer->Add(autoStuff, 1, wxEXPAND);

        sizer->Add(new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxSize(1, 1), wxLI_HORIZONTAL), 0,
                   wxEXPAND | wxLEFT | wxRIGHT, 20);

        ioStuff = new ioStuffPanel(this, settings["serialPort"]["baud"].get_ref<std::string&>());
        sizer->Add(ioStuff, 1, wxEXPAND);
    }catch(json::exception& e){
        wxMessageBox("config file is corrupted/malformed. please delete it and restart the software to generate a clean one", "config file parsing exception", wxICON_ERROR);
    }

    this->SetSizerAndFit(sizer);
}

/*
 * following three methods are just forwarders to expose ioStuff/autoStuff panels to mainFrame
 */

void connectionPanel::handleNewComs(const std::vector<std::wstring>& coms) {
    ioStuff->populateComCombo(coms);
}

void connectionPanel::lookForVidPid(std::wstring &vid, std::wstring &pid, const bool autostart, const bool autolog) {
    autoStuff->autoVidPid(vid, pid, autostart, autolog);
}

void connectionPanel::setPortStatusVerboseMessage(std::wstring s) {
    ioStuff->setPortStatusVerboseMessage(s);
}

/*
 * autoVidPid actual implementation. the interface w/ Windows registry and Windows COMs is hw_detection.h/cpp
 * this method forwards values to mainFrame in the form of wxEvents (which are thread-safe!)
 */
void autoStuffPanel::autoVidPid(std::wstring& vid, std::wstring& pid, const bool autoconnect, const bool autolog) {
    std::wstring autoFindVerbose;
    if(!vid.empty() && !pid.empty()){
        try{
            const auto com = LookForVidPidCom(vid,pid);

            if(com.has_value()){
                autoFindVerbose += com.value();
                autoFindVerbose += L" automatically found on USB (";
                autoFindVerbose += vid + "," + pid;
                autoFindVerbose += L")";


                {   //*** select COM ***
                    auto *combo = (wxComboBox *) FindWindowById(ID_Conn_COMCombobox, this->GetParent());
                    combo->SetSelection(combo->FindString(com.value()));
                    auto* ev = new wxCommandEvent(wxEVT_COMBOBOX, ID_Conn_COMCombobox);
                    ev->SetEventObject(combo);
                    combo->GetEventHandler()->QueueEvent(ev);
                }

                if(autoconnect){
                    {
                        auto *connectBtn = (wxButton *) FindWindowById(ID_Conn_COMOpenClose, this->GetParent());
                        if(connectBtn->GetLabelText().ToStdString() == "OPEN") {
                            auto* ev = new wxCommandEvent(wxEVT_BUTTON, ID_Conn_COMOpenClose);
                            ev->SetEventObject(connectBtn);
                            ev->SetInt(AUTO_TRIGGERED);
                            connectBtn->GetEventHandler()->QueueEvent(ev);
                        }
                    }

                    if(autolog){
                        auto* startBtn = (wxToggleButton*) FindWindowById(ID_Log_StartStop, nullptr);
                        auto* ev = new wxCommandEvent(wxEVT_TOGGLEBUTTON, ID_Log_StartStop);
                        ev->SetEventObject(startBtn);
                        ev->SetInt(AUTO_TRIGGERED);
                        startBtn->GetEventHandler()->QueueEvent(ev);
                    }
                }
            }else{
                autoFindVerbose = L"COM not (yet) found by USB enumeration";
            }
        }catch(winreg::RegException &e){
            wxMessageBox("failed to access windows registry", "windows registry access error", wxICON_ERROR);
        }
    }

    autoFindVerboseMessage->SetLabel(autoFindVerbose);
}

/*
 * autoStuff panel constructor
 * just GUI construction
 */
autoStuffPanel::autoStuffPanel(wxWindow *parent, const bool autoconnect, const bool autolog) : wxPanel(parent) {
    sizer->AddGrowableCol(0, 1);
    sizer->AddGrowableCol(1, 1);

    wxSizer* autoConnectSizer = new wxBoxSizer(wxVERTICAL);

    autoConnectSizer->Add(CreateSizerWithTextAndToggleButton(this, ID_Conn_Auto_ToggleConn, "AUTO-CONNECT ", autoconnect), 1, wxEXPAND);
    autoConnectSizer->Add(CreateSizerWithTextAndToggleButton(this, ID_Conn_Auti_ToggleLog, "AUTO-LOG ", autolog), 1, wxEXPAND);

    autoFindVerboseMessage = new wxStaticText(this, wxID_ANY, wxEmptyString);

    sizer->Add(autoFindVerboseMessage, 1, wxEXPAND | wxALL, 20);
    sizer->Add(autoConnectSizer, 1, wxEXPAND | wxALL, 20);

    this->SetSizerAndFit(sizer);
}

/*
 * ioStuff panel constructor
 * just GUI construction
 */
ioStuffPanel::ioStuffPanel(wxWindow *parent, std::string& baud) : wxPanel(parent){
    sizer->AddGrowableCol(0, 1);
    sizer->AddGrowableCol(1, 0);

    wxSizer* comAndBaudSizer = new wxBoxSizer(wxVERTICAL);
    wxSizer* portStatusSizer = new wxBoxSizer(wxVERTICAL);

    comCombo = new wxComboBox(this, ID_Conn_COMCombobox);
    try {
        populateComCombo(EnumComs());
    } catch(winreg::RegException &e){
        wxMessageBox("failed to access windows registry", "windows registry access error", wxICON_ERROR);
    }

    auto* baudChooser = new wxChoice(this, ID_Conn_BaudChoice);
    PopulateItemContainerWithIntVector(baudChooser, BAUD_RATES);
    baudChooser->SetStringSelection(baud);
    auto* ev = new wxCommandEvent(wxEVT_CHOICE, ID_Conn_BaudChoice);
    ev->SetEventObject(baudChooser);
    baudChooser->GetEventHandler()->QueueEvent(ev);

    comAndBaudSizer->Add(CreateSizerWithTextAndCombobox(this, comCombo, "PORT "), 1, wxEXPAND | wxBOTTOM, 5);
    comAndBaudSizer->Add(CreateSizerWithTextAndDropdown(this, baudChooser, "BAUD "), 1, wxEXPAND | wxTOP, 5);

    portStatusVerboseMessage = new wxStaticText(this, wxID_ANY, "port CLOSED");

    portStatusSizer->Add(portStatusVerboseMessage, 1, wxEXPAND | wxCENTER);
    portStatusSizer->Add(new wxButton(this, ID_Conn_COMOpenClose, "OPEN"), 0, wxEXPAND);

    sizer->Add(comAndBaudSizer, 1, wxEXPAND | wxALL, 20);
    sizer->Add(portStatusSizer, 1, wxEXPAND | wxALL, 20);

    this->SetSizerAndFit(sizer);
}

void ioStuffPanel::setPortStatusVerboseMessage(std::wstring& s) {
    portStatusVerboseMessage->SetLabel(s);
    this->Layout();
}

void ioStuffPanel::populateComCombo(const std::vector<std::wstring>& values) {
    comCombo->Clear();

    for(auto& val: values)
        comCombo->Append(val);
}