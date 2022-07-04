/*
 * microRadar software
 * (progetto polimi 2022)
 *
 * Salvatore Passaro
 */

#ifndef MICRORADAR_ABOUTDIALOG_H
#define MICRORADAR_ABOUTDIALOG_H

#include <wx/wx.h>

class aboutDialog : public wxDialog{
    wxSizer* sizer = new wxBoxSizer(wxVERTICAL);

public:
    explicit aboutDialog(wxWindow* parent);
};

#endif //MICRORADAR_ABOUTDIALOG_H