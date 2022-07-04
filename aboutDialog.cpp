/*
 * microRadar software
 * (progetto polimi 2022)
 *
 * Salvatore Passaro
 */

#include "aboutDialog.h"

aboutDialog::aboutDialog(wxWindow *parent) : wxDialog(parent, wxID_ANY, "about this software", wxDefaultPosition, wxDefaultSize, wxSYSTEM_MENU | wxFRAME_FLOAT_ON_PARENT | wxCAPTION | wxCLOSE_BOX) {
    auto icon = wxBitmap(256,256);
    icon.CopyFromIcon(wxICON(AAAicon));

    sizer->Add(new wxStaticBitmap(this, wxID_ANY, icon),1,wxEXPAND);
    sizer->Add(new wxStaticText(this, wxID_ANY, "microRadar parser&&logger"), 1, wxCENTER | wxLEFT | wxRIGHT, 60);
    sizer->Add(new wxStaticText(this, wxID_ANY, "this software is specifically wrote to interface to a SmartMicro UMRR-0A0703-220701- 050704 in RS485 mode"), 1, wxCENTER | wxALL, 10);

    sizer->Add(new wxStaticText(this, wxID_ANY, "please, contact me at salvatore.passaro@mail.polimi.it if something doesn't work as expected"), 1, wxCENTER | wxALL, 10);
    this->SetSizerAndFit(sizer);
    this->ShowModal();
}