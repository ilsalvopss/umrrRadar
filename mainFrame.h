/*
 * microRadar software
 * (progetto polimi 2022)
 *
 * Salvatore Passaro
 */

#ifndef MICRORADAR_MAINFRAME_H
#define MICRORADAR_MAINFRAME_H

#include <wx/wx.h>
#include <string>
#include "connectionPanel.h"
#include "measurePanel.h"
#include "logPanel.h"
#include "settingsDialog.h"
#include "hw_detection.h"
#include "persistenceManager.h"
#include "phyConnectionManager.h"
#include "filesystemLogManager.h"
#include "gui_helpers.h"
#include "aboutDialog.h"

#ifdef _WIN32
#include <dbt.h>
#include "usbiodef.h"
#endif

class mainFrame: public wxFrame
{
    connectionPanel* connPanel;
    measurePanel* mPanel;
    logPanel* lPanel;
    persistenceManager persistence;
    phyConnectionManager phyConnManager;
    filesystemLogManager fsLogManager;
    std::mutex vidpidlock;

public:
    mainFrame(const wxString& title, const wxPoint& pos);

private:
    void buildMenuBar();
    void OnSettings(wxCommandEvent& event);
    void OnExit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void onConnAutoToggle(wxCommandEvent& event);
    void onComOpenClose(wxCommandEvent& event);
    void onBaudChoice(wxCommandEvent& event);
    void onComSelection(wxCommandEvent& event);
    void onStartStopLogging(wxCommandEvent& event);
    void onErrorCallback(wxCommandEvent& event);
    void requestVidPid();
    void subscribeToOsNotifications();

#ifdef _WIN32
    virtual WXLRESULT MSWWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam);
#endif

    wxDECLARE_EVENT_TABLE();
};

#endif //MICRORADAR_MAINFRAME_H