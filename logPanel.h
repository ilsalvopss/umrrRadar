/*
 * microRadar software
 * (progetto polimi 2022)
 *
 * Salvatore Passaro
 */

#ifndef MICRORADAR_LOGPANEL_H
#define MICRORADAR_LOGPANEL_H

#include "wx/wx.h"
#include "gui_helpers.h"
#include "persistenceManager.h"
#include <wx/filepicker.h>
#include <chrono>

class logPanel : public wxPanel{
    wxSizer *const sizer = new wxStaticBoxSizer(wxHORIZONTAL, this, "log");

public:
    explicit logPanel(wxWindow* parent, persistenceManager&);
};

#endif //MICRORADAR_LOGPANEL_H