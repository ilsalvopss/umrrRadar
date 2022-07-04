/*
 * microRadar software
 * (progetto polimi 2022)
 *
 * Salvatore Passaro
 */

#include "gui_helpers.h"

/*
 * this is a set of helper methods to facilitate GUI building and avoid code duplications
 */

std::string to_narrow(std::wstring& wide_string){
    static std::wstring_convert<std::codecvt_utf8<wchar_t>> utf8_conv;
    return utf8_conv.to_bytes(wide_string);
}

std::wstring to_wide(std::string& str){
    static std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    return converter.from_bytes(str);
}

void MakeControlFontBold(wxControl* ctrl){
    wxFont f = ctrl->GetFont();
    f.MakeBold();
    ctrl->SetFont(f);
}

void MakeControlFontSmaller(wxControl* ctrl){
    wxFont f = ctrl->GetFont();
    f.MakeSmaller();
    ctrl->SetFont(f);
}

void MakeControlFontMonospace(wxControl* ctrl){
    wxFont f = ctrl->GetFont();
    f.SetFamily(wxFONTFAMILY_TELETYPE);
    ctrl->SetFont(f);
}

void SetControlFontSize(wxControl* ctrl, double size){
    wxFont f = ctrl->GetFont();
    f.SetFractionalPointSize(size);
    ctrl->SetFont(f);
}

wxSizer *CreateSizerWithTextAndToggleButton(wxWindow* parent, wxWindowID btnId, const char* label, bool btnInitialState = false) {
    wxSizer *sizerRow = new wxBoxSizer(wxHORIZONTAL);
    auto *text = new wxStaticText(parent, wxID_ANY, label);

    MakeControlFontBold(text);
    MakeControlFontSmaller(text);

    auto* btn = new wxToggleButton(parent, btnId, btnInitialState ? "ON" : "OFF", wxDefaultPosition, wxDefaultSize ,wxBU_EXACTFIT);
    btn->SetValue(btnInitialState);

    MakeControlFontBold(btn);
    MakeControlFontSmaller(btn);

    sizerRow->Add(text, 1, wxEXPAND);
    sizerRow->Add(btn, 0);

    return sizerRow;
}

wxSizer *CreateSizerWithTextAndDropdown(wxWindow* parent, wxChoice* choice, const char* label, bool smaller){
    wxSizer *sizerRow = new wxBoxSizer(wxHORIZONTAL);
    auto *text = new wxStaticText(parent, wxID_ANY, label);

    MakeControlFontBold(text);
    MakeControlFontBold(choice);

    if(smaller){
        MakeControlFontSmaller(text);
        MakeControlFontSmaller(choice);
    }

    sizerRow->Add(text, 1, wxEXPAND);
    sizerRow->Add(choice, 0);

    return sizerRow;
}

wxSizer *CreateSizerWithTextAndCombobox(wxWindow* parent, wxComboBox* comboBox, const char* label){
    wxSizer *sizerRow = new wxBoxSizer(wxHORIZONTAL);
    auto *text = new wxStaticText(parent, wxID_ANY, label);

    MakeControlFontBold(text);

    MakeControlFontBold(comboBox);

    sizerRow->Add(text, 1, wxEXPAND);
    sizerRow->Add(comboBox, 0);

    return sizerRow;
}

wxSizer *CreateSizerWithTextAndSpin(wxWindow* parent, wxWindowID spinId, const char* label, int initial_value, bool smaller){
    auto* sizerRow = new wxBoxSizer(wxHORIZONTAL);
    auto* text = new wxStaticText(parent, wxID_ANY, label);

    auto* spin = new wxSpinCtrl(parent, spinId, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, INT_MAX, initial_value);

    MakeControlFontBold(text);
    MakeControlFontBold(spin);

    if(smaller){
        MakeControlFontSmaller(text);
        MakeControlFontSmaller(spin);
    }

    sizerRow->Add(text, 1, wxEXPAND);
    sizerRow->Add(spin, 0);

    return sizerRow;
}

wxTextCtrl* makeTextCtrlMinSize(wxTextCtrl* ctrl, int len){
    ctrl->SetSizeHints(ctrl->GetTextExtent("199999"), ctrl->GetTextExtent("199999"));
    return ctrl;
}

wxCheckBox* CreateCheckBox(wxWindow* parent, wxWindowID id, bool state, const char* label){
    auto* cbox = new wxCheckBox(parent, id, label);
    cbox->SetValue(state);
    return cbox;
}

void PopulateItemContainerWithIntVector(wxItemContainer* c, const std::vector<int>& v){
    for(int i: v)
        c->Append(std::to_string(i));
}