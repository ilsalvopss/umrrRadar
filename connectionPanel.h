/*
 * microRadar software
 * (progetto polimi 2022)
 *
 * Salvatore Passaro
 */

#ifndef MICRORADAR_CONNECTIONPANEL_H
#define MICRORADAR_CONNECTIONPANEL_H

#include "wx/wx.h"
#include "gui_helpers.h"
#include <wx/statline.h>
#include "hw_detection.h"
#include "persistenceManager.h"
#include <locale>
#include <codecvt>

class autoStuffPanel : public wxPanel{
    wxFlexGridSizer *const sizer = new wxFlexGridSizer(2);
    wxStaticText* autoFindVerboseMessage;

public:
    explicit autoStuffPanel(wxWindow* parent, bool, bool);
    void autoVidPid(std::wstring&, std::wstring&, bool, bool);
};

class ioStuffPanel : public wxPanel{
    wxFlexGridSizer *const sizer = new wxFlexGridSizer(2);
    wxStaticText* portStatusVerboseMessage;
    wxComboBox* comCombo;

public:
    explicit ioStuffPanel(wxWindow* parent, std::string&);
    void setPortStatusVerboseMessage(std::wstring&);
    void populateComCombo(const std::vector<std::wstring>&);

};

class connectionPanel : public wxPanel{
    wxSizer *const sizer = new wxStaticBoxSizer(wxVERTICAL, this, "connection");

    autoStuffPanel* autoStuff;
    ioStuffPanel* ioStuff;

public:
    explicit connectionPanel(wxWindow* parent, persistenceManager&);
    void handleNewComs(const std::vector<std::wstring>&);
    void lookForVidPid(std::wstring &vid, std::wstring &pid, bool, bool);
    void setPortStatusVerboseMessage(std::wstring);
};
#endif //MICRORADAR_CONNECTIONPANEL_H