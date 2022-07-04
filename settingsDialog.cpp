/*
 * microRadar software
 * (progetto polimi 2022)
 *
 * Salvatore Passaro
 */

#include "settingsDialog.h"

wxBEGIN_EVENT_TABLE(settingsPanel, wxPanel)
                EVT_BUTTON(ID_SettingsSave, settingsPanel::handleSave)
                EVT_TOGGLEBUTTON(ID_SettingsAutoConnectToggle, settingsPanel::handleToggle)
                EVT_TOGGLEBUTTON(ID_SettingsAutoLogToggle, settingsPanel::handleToggle)
wxEND_EVENT_TABLE()

/*
 * settingsDialog constructor,
 * passes reference to persistenceManager through to settingsPanel
 */
settingsDialog::settingsDialog(wxWindow *parent, persistenceManager& p) : wxDialog(parent, wxID_ANY, "persistent settings", wxDefaultPosition, wxDefaultSize, wxSYSTEM_MENU | wxFRAME_FLOAT_ON_PARENT | wxCAPTION | wxCLOSE_BOX) {
    this->SetBackgroundColour(wxColor(240,240,240));

    try {
        mainPanel = new settingsPanel(this, p);
    }catch(std::runtime_error& e){
        this->Destroy();
        return;
    }

    sizer->Add(new wxStaticText(this, wxID_ANY, "these settings are dumped to the filesystem and loaded at startup"), 0, wxEXPAND | wxALL, 15);
    sizer->Add(new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxSize(0,1)), 0, wxEXPAND | wxALL, 15);
    sizer->Add(mainPanel, 1, wxEXPAND | wxALL, 20);

    SetSizerAndFit(sizer);
    this->ShowModal();
}

void settingsPanel::handleToggle(wxCommandEvent &event) {
    auto* btn = (wxToggleButton*) event.GetEventObject();
    btn->SetLabel(btn->GetValue() ? "ON" : "OFF");
}

/*
 * pulls data from the gui and passes it to the persistenceManager,
 * then dumps it to filesystem
 */
void settingsPanel::handleSave(wxCommandEvent &event) {
    for(const auto& e : WidgetsPersistenceMapStr){
        auto el = ((wxTextCtrl *) FindWindowById(e.first, this))->GetValue().ToStdString();
        persistence.setByPath(json::json_pointer(e.second), el);
    }

    for(const auto& e : WidgetsPersistenceMapBool){
        auto el = ((wxToggleButton *) FindWindowById(e.first, this))->GetValue();
        persistence.setByPath(json::json_pointer(e.second), el);
    }

    auto baud = ((wxChoice *) FindWindowById(ID_SettingsDefaultBaud, this))->GetStringSelection().ToStdString();
    persistence.setByPath(json::json_pointer("/connectionSettings/serialPort/baud"), baud);

    auto period = ((wxSpinCtrl *) FindWindowById(ID_SettingsAveragePeriod, this))->GetValue();
    persistence.setByPath(json::json_pointer("/logSettings/averagePeriod"), period);

    persistence.dumpRoot();

    this->GetParent()->Destroy();
}

settingsPanel::settingsPanel(wxWindow *parent, persistenceManager& p) : persistence(p), wxPanel(parent){
    sizer->AddGrowableCol(0, 1);
    sizer->AddGrowableCol(1, 1);

    try {
        auto connPersistence = persistence.getConnectionSettings();

        sizer->Add(CreateSizerWithTextAndToggleButton(this, ID_SettingsAutoConnectToggle, "USB auto-connect ",
                                                      connPersistence["autoOpenConnection"]), 1, wxEXPAND | wxALL, 15);
        sizer->Add(new wxStaticText(this, wxID_ANY, "when enabled, serial connection will automatically be opened"), 1, wxEXPAND | wxALL,
                   15);

        sizer->Add(CreateSizerWithTextAndToggleButton(this, ID_SettingsAutoLogToggle, "USB auto-log ",
                                                      connPersistence["autoStartLogging"]), 1, wxEXPAND | wxALL, 15);
        sizer->Add(new wxStaticText(this, wxID_ANY, "when enabled, logging will auto-start after the connection is opened"), 1, wxEXPAND | wxALL,
                   15);

        auto* vidpidsizer = new wxBoxSizer(wxHORIZONTAL);
        auto* vidpidlabel = new wxStaticText(this, wxID_ANY, "USB VID.PID");
        MakeControlFontBold(vidpidlabel);
        MakeControlFontSmaller(vidpidlabel);
        vidpidsizer->Add(vidpidlabel, 1, wxEXPAND);
        vidpidsizer->Add(makeTextCtrlMinSize(new wxTextCtrl(this, ID_SettingsVid, connPersistence["usbAutoDetection"]["vid"]), 4), 0, wxRIGHT, 4);
        vidpidsizer->Add(makeTextCtrlMinSize(new wxTextCtrl(this, ID_SettingsPid, connPersistence["usbAutoDetection"]["pid"]), 4));

        sizer->Add(vidpidsizer, 1, wxEXPAND | wxALL, 15);
        sizer->Add(new wxStaticText(this, wxID_ANY, "VID/PID parameters of USB serial adapter"), 1, wxEXPAND | wxALL,
                   15);

        auto *dropdown = new wxChoice(this, ID_SettingsDefaultBaud);
        PopulateItemContainerWithIntVector(dropdown, BAUD_RATES);
        dropdown->SetStringSelection(connPersistence["serialPort"]["baud"]);

        sizer->Add(CreateSizerWithTextAndDropdown(this, dropdown, "BAUD RATE ", true), 1, wxEXPAND | wxALL, 15);
        sizer->Add(new wxStaticText(this, wxID_ANY, "default serial baud-rate"), 1, wxEXPAND | wxALL,
                   15);

        auto logPersistence = persistence.getLogSettings();

        sizer->Add(CreateSizerWithTextAndSpin(this, ID_SettingsAveragePeriod, "average period [ms] ",
                                              logPersistence["averagePeriod"], true), 1, wxEXPAND | wxALL, 15);
        sizer->Add(new wxStaticText(this, wxID_ANY, "average period in milliseconds"), 1, wxEXPAND | wxALL,
                   15);

        sizer->AddStretchSpacer(1);
        sizer->Add(new wxButton(this, ID_SettingsSave, "save"), 1, wxEXPAND);
    }catch(json::exception& e){
        wxMessageBox("config file is corrupted/malformed. please delete it and restart the software to generate a clean one", "config file parsing exception", wxICON_ERROR);
        throw std::runtime_error("json parsing");
    }

    this->SetSizerAndFit(sizer);
}