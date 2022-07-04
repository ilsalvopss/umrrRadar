/*
 * microRadar software
 * (progetto polimi 2022)
 *
 * Salvatore Passaro
 */

#ifndef MICRORADAR_SETTINGSDIALOG_H
#define MICRORADAR_SETTINGSDIALOG_H

#include <wx/wx.h>
#include <wx/statline.h>
#include <map>
#include "gui_helpers.h"
#include <wx/filepicker.h>
#include "persistenceManager.h"

enum {
    ID_SettingsAutoConnectToggle,
    ID_SettingsAutoLogToggle,
    ID_SettingsDefaultBaud,
    ID_SettingsAveragePeriod,
    ID_SettingsVid,
    ID_SettingsPid,
    ID_SettingsSave
};

class settingsPanel : public wxPanel{
    wxFlexGridSizer *const sizer = new wxFlexGridSizer(2);
    persistenceManager& persistence;

    const std::map<const wxWindowID, const std::string> WidgetsPersistenceMapBool = {
            {ID_SettingsAutoLogToggle, "/connectionSettings/autoStartLogging"},
            {ID_SettingsAutoConnectToggle, "/connectionSettings/autoOpenConnection"}
    };

    const std::map<const wxWindowID, const std::string> WidgetsPersistenceMapStr = {
            {ID_SettingsVid, "/connectionSettings/usbAutoDetection/vid"},
            {ID_SettingsPid, "/connectionSettings/usbAutoDetection/pid"}
    };

    void handleSave(wxCommandEvent& event);
    void handleToggle(wxCommandEvent& event);

public:
    settingsPanel(wxWindow* parent, persistenceManager&);

wxDECLARE_EVENT_TABLE();
};

class settingsDialog : public wxDialog{
    wxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    settingsPanel* mainPanel;

public:
    explicit settingsDialog(wxWindow* parent, persistenceManager&);
};

#endif //MICRORADAR_SETTINGSDIALOG_H