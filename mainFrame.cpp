/*
 * microRadar software
 * (progetto polimi 2022)
 *
 * Salvatore Passaro
 */

#include "mainFrame.h"

/*
 * wxWidgets event table
 * event IDs are declared in gui_helpers.h
 */
wxBEGIN_EVENT_TABLE(mainFrame, wxFrame)
                EVT_TOGGLEBUTTON(ID_Conn_Auto_ToggleConn, mainFrame::onConnAutoToggle)
                EVT_TOGGLEBUTTON(ID_Conn_Auti_ToggleLog, mainFrame::onConnAutoToggle)
                EVT_BUTTON(ID_Conn_COMOpenClose, mainFrame::onComOpenClose)
                EVT_CHOICE(ID_Conn_BaudChoice, mainFrame::onBaudChoice)
                EVT_COMBOBOX(ID_Conn_COMCombobox, mainFrame::onComSelection)
                EVT_TOGGLEBUTTON(ID_Log_StartStop, mainFrame::onStartStopLogging)
                EVT_COMMAND(ID_Callback_Error, ERROR_EVENT, mainFrame::onErrorCallback)
                EVT_MENU(ID_Settings, mainFrame::OnSettings)
                EVT_MENU(wxID_EXIT, mainFrame::OnExit)
                EVT_MENU(wxID_ABOUT, mainFrame::OnAbout)
wxEND_EVENT_TABLE()

/*
 * constructor of mainFrame,
 *
 * other than GUI business it:
 *  - subscribes to Windows device change notifications
 *  - keeps references to *Managers
 *  - listens for events from children and even other threads (note that exEvents are thread safe)
 */
mainFrame::mainFrame(const wxString& title, const wxPoint& pos)
        : wxFrame(nullptr, wxID_ANY, title, pos) {

    wxBusyCursor wait;

    subscribeToOsNotifications();

#if defined(__WXMSW__)
	this->SetIcon(wxICON(AAAicon));
#endif

    this->SetBackgroundColour(wxColor(240,240,240));
    buildMenuBar();

    wxSizer* const main_sizer = new wxBoxSizer(wxVERTICAL);

    connPanel = new connectionPanel(this, persistence);
    main_sizer->Add(connPanel, 1, wxEXPAND | wxALL, 20);

    mPanel = new measurePanel(this);
    main_sizer->Add(mPanel, 1, wxEXPAND | wxALL, 20);

    lPanel = new logPanel(this, persistence);
    main_sizer->Add(lPanel, 1, wxEXPAND | wxALL, 20);

    SetSizerAndFit(main_sizer);

    requestVidPid();
}

void mainFrame::buildMenuBar() {
    auto *menuBar = new wxMenuBar;

    auto *menuSetup = new wxMenu;
    menuSetup->Append(ID_Settings, "&Settings...\tCtrl-S", "Global app settings.");
    menuSetup->AppendSeparator();
    menuSetup->Append(wxID_EXIT);

    auto *menuHelp = new wxMenu;
    menuHelp->Append(wxID_ABOUT);

    menuBar->Append( menuSetup, "&Setup" );
    menuBar->Append( menuHelp, "&Help" );

    this->SetMenuBar(menuBar);
}

/*
 * wrapper around Windows API RegisterDeviceNotification
 *
 * note: we just subscribe to USB device changes
 */
void mainFrame::subscribeToOsNotifications() {
#ifdef _WIN32
    DEV_BROADCAST_DEVICEINTERFACE NotificationFilter;

    ZeroMemory(&NotificationFilter, sizeof(NotificationFilter));
    NotificationFilter.dbcc_size = sizeof(DEV_BROADCAST_DEVICEINTERFACE);
    NotificationFilter.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
    NotificationFilter.dbcc_classguid = GUID_DEVINTERFACE_USB_DEVICE;

    RegisterDeviceNotification(
        this->GetHandle(),          // events recipient
        &NotificationFilter,        // type of device
        DEVICE_NOTIFY_WINDOW_HANDLE // type of recipient handle
    );
#endif
}

/*
 * handles settings button and keystroke
 * creates a settings dialog.
 *
 * the dialog auto-destructs itself on save/close events
 */
void mainFrame::OnSettings(wxCommandEvent& event) {
    new settingsDialog(this, persistence);
}

/*
 * overrides wxWidgets messages handling entry point to catch Windows's native WM_DEVICECHANGE
 * when caught WM_DEVICECHANGE triggers:
 * - a com list update in GUI
 * - an usb vid/pid auto-scan (handled in connectionPanel class)
 *
 * all other messages are forwarded to wxWidgets internals
 */
#ifdef _WIN32
WXLRESULT mainFrame::MSWWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam) {
    if (nMsg == WM_DEVICECHANGE) {
        if(!phyConnManager.isPhyOpen()){
            try {
                connPanel->handleNewComs(EnumComs());

                requestVidPid();
            } catch (winreg::RegException &e) {
                wxMessageBox("failed to access windows registry", "windows registry access error", wxICON_ERROR);
            }
        }
    }

    return wxFrame::MSWWindowProc(nMsg, wParam, lParam);
}
#endif

void mainFrame::OnExit(wxCommandEvent& event) {
    Close(true);
}

void mainFrame::OnAbout(wxCommandEvent& event) {
    new aboutDialog(this);
}

/*
 * handles auto* toggleButtons;
 * forwards values to persistenceManager immediately
 */
void mainFrame::onConnAutoToggle(wxCommandEvent& event){
    auto* btn = (wxToggleButton*) event.GetEventObject();
    const bool state = btn->GetValue();

    switch (event.GetId()) {
        case ID_Conn_Auto_ToggleConn:
            persistence.setAutoToggle(state);
            break;

        case ID_Conn_Auti_ToggleLog:
            persistence.setAutoToggle(std::nullopt, state);
            break;
    }

    btn->SetLabel(state ? "ON" : "OFF");
}

/*
 * event handler for error messages from serial listener/filesystem logger threads
 */
void mainFrame::onErrorCallback(wxCommandEvent &event) {
    switch (event.GetInt()) {
        case Phy_Failed: {
            auto *ocBtn = FindWindowById(ID_Conn_COMOpenClose, connPanel);

            ocBtn->SetLabel(!phyConnManager.isPhyOpen() ? "OPEN" : "CLOSE");
            connPanel->setPortStatusVerboseMessage(L"serial error, connection closed");
        }
        break;

        case FS_Failed:{

        }
        break;
    }
}

/*
 * event handler for user request to open connection OR autoVidPid trigger
 *
 * if the event has int field set to AUTO_TRIGGERED than it was generated by autoVidPid (=not by the user)
 * so we avoid stop_listening, nor we close the phy layer
 * (this is because Windows triggers multiple useless events when a device change happens)
 */
void mainFrame::onComOpenClose(wxCommandEvent &event) {
    wxBusyCursor wait;
    wxWindowDisabler _winwait;

    auto* btn = (wxButton*) event.GetEventObject();
    bool justStarted = false;

    if(!phyConnManager.isPhyOpen()) {
        phyConnManager.open_phy([this](const ErrorID id, const std::optional<std::string>& desc){
            auto* evt = new wxCommandEvent(ERROR_EVENT, ID_Callback_Error);
            evt->SetInt(id);
            if(desc.has_value())
                evt->SetString(desc.value());
            this->GetEventHandler()->QueueEvent(evt);
        });

        justStarted = true;
    } else if(event.GetInt() != AUTO_TRIGGERED) {
        phyConnManager.stop_listening();
        phyConnManager.close_phy();
    }

    btn->SetLabel(!phyConnManager.isPhyOpen() ? "OPEN" : "CLOSE");

    connPanel->setPortStatusVerboseMessage(phyConnManager.getCom() +
                                           (!phyConnManager.isPhyOpen() ? L" CLOSED" : L" OPEN at " +
                                                                                       std::to_wstring(phyConnManager.getBaud()) +
                                                                                       L" baud rate"));

    if(justStarted && phyConnManager.isPhyOpen()){
        std::function<void(umrrMessage)> cb = [this](umrrMessage m){
            if(m.altitude_valid) {
                mPanel->updateMeasure(m.altitude);

                fsLogManager.handle_measure(m);
            }
        };

        phyConnManager.start_listening(cb);
    }
}

/*
 * handler for baud choosing. this can be triggered only by the user (autoVidPid doesn't make assumptions on baud rates)
 * the value is immediately forwarded to phyConnectionManager
 */
void mainFrame::onBaudChoice(wxCommandEvent& event) {
    auto* choice = (wxChoice*) event.GetEventObject();
    if(phyConnManager.isPhyOpen()){
        if(phyConnManager.getBaud() != 0) {
            const int s = choice->FindString(std::to_wstring(phyConnManager.getBaud()));
            if(s != wxNOT_FOUND)
                choice->SetSelection(s);
        }
        return;
    }

    try {
        phyConnManager.setBaud(std::stoi(choice->GetStringSelection().ToStdString()));
    }catch(std::invalid_argument& e){
        std::cerr << "[BUG] this should not happen" << std::endl;
    }
}

/*
 * handler for com selection. this could be triggered by the user or AutoVidPid
 * the value is immediately forwarded to phyConnectionManager
 */
void mainFrame::onComSelection(wxCommandEvent &event) {
    wxWindowDisabler _winwait;

    auto* combo = (wxComboBox*) event.GetEventObject();
    if(phyConnManager.isPhyOpen()){
        if(!phyConnManager.getCom().empty()) {
            const int s = combo->FindString(phyConnManager.getCom());
            if(s != wxNOT_FOUND)
                combo->SetSelection(s);
        }
        return;
    }

    auto com = combo->GetStringSelection().ToStdWstring();
    phyConnManager.setCom(com);
}

/*
 * handler for start/stop logging button
 * logging options are pulled from the GUI and passed to filesystemLogManager
 */
void mainFrame::onStartStopLogging(wxCommandEvent &event) {
    auto* btn = (wxToggleButton *) event.GetEventObject();

    if(!fsLogManager.getisLogging()) {
        auto *out_f = (wxFilePickerCtrl *) FindWindowById(ID_Log_FilePicker, btn->GetParent());

        if (!fsLogManager.getisOpen() || fsLogManager.getOutPath() != std::filesystem::path(out_f->GetPath().ToStdString())) {
            if(wxIsEmpty(out_f->GetPath())){
                wxMessageBox(
                        "please specify an output path before start logging.",
                        "no output path", wxICON_WARNING);
                const auto actual = fsLogManager.getisLogging();
                btn->SetValue(actual);
                btn->SetLabel(actual ? "STOP" : "START");
                return;
            }

            fsLogManager.openOut(std::filesystem::path(out_f->GetPath().ToStdString()));
        }

        if(fsLogManager.getisDirty()){
            auto warning = new wxMessageDialog(this, "Appending to a file already used for logging. Continue?", "Reusing file", wxYES_NO | wxICON_WARNING | wxCENTER);
            if(warning->ShowModal() != wxID_YES) {
                const auto actual = fsLogManager.getisLogging();
                btn->SetValue(actual);
                btn->SetLabel(actual ? "STOP" : "START");
                return;
            }
        }

        auto *avg_period = (wxSpinCtrl *) FindWindowById(ID_Log_AvgPeriod, btn->GetGrandParent());
        modularFlags flags = NONE;
        flags = (modularFlags)(flags | (((wxCheckBox *) FindWindowById(ID_Log_ModularTIME, btn->GetGrandParent()))->GetValue() ? TIME : NONE));
        flags = (modularFlags)(flags | (((wxCheckBox *) FindWindowById(ID_Log_ModularAVG, btn->GetGrandParent()))->GetValue() ? AVG : NONE));
        flags = (modularFlags)(flags | (((wxCheckBox *) FindWindowById(ID_Log_ModularDEV, btn->GetGrandParent()))->GetValue() ? DEV : NONE));
        flags = (modularFlags)(flags | (((wxCheckBox *) FindWindowById(ID_Log_ModularSEM, btn->GetGrandParent()))->GetValue() ? SEM : NONE));

        auto avg_mode = ((wxRadioBox *) FindWindowById(ID_Log_Avg, btn->GetGrandParent()))->GetStringSelection().ToStdString();

        auto error_lambda = [](ErrorID, const std::optional<std::string>&) {
            wxMessageBox(
                    "A filesystem error happened. We probably cannot recover normal operation",
                    "fs error", wxICON_WARNING);
        };

        const auto period_millis = std::chrono::milliseconds (avg_period->GetValue());

        fsLogManager.startLogging(period_millis, avgAccumulators.at(avg_mode), error_lambda, flags);

    }else if(event.GetInt() != AUTO_TRIGGERED){
        fsLogManager.stopLogging();
    }

    {
        auto actual = fsLogManager.getisLogging();
        btn->SetValue(actual);
        btn->SetLabel(actual ? "STOP" : "START");
    }
}

/*
 * begin autoVidPid detection which is handled inside connPanel
 */
void mainFrame::requestVidPid() {
    std::unique_lock<std::mutex> _l(vidpidlock);

    wxBusyCursor wait;
    wxWindowDisabler _winwait;

    try {
        json settings = persistence.getConnectionSettings();

        auto _vid = to_wide(settings["usbAutoDetection"]["vid"].get_ref<std::string &>());
        auto _pid = to_wide(settings["usbAutoDetection"]["pid"].get_ref<std::string &>());

        connPanel->lookForVidPid(_vid, _pid, settings["autoOpenConnection"], settings["autoStartLogging"]);
    } catch (json::exception &e) {
        wxMessageBox(
                "config file is corrupted/malformed. please delete it and restart the software to generate a clean one",
                "config file parsing exception", wxICON_ERROR);
    }
}