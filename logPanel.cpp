/*
 * microRadar software
 * (progetto polimi 2022)
 *
 * Salvatore Passaro
 */

#include "logPanel.h"

/*
 * constructs logPanel, pulling defaults from persistenceManager reference
 */
logPanel::logPanel(wxWindow *parent, persistenceManager& persistence) : wxPanel(parent){
    auto* freq_sizer = new wxBoxSizer(wxVERTICAL);
    auto* control_sizer = new wxBoxSizer(wxVERTICAL);

    auto* avg_sizer = new wxBoxSizer(wxHORIZONTAL);
    wxArrayString avgChoices;
    for(auto& e : avgAccumulators)
        avgChoices.Add(e.first);

    avg_sizer->Add(new wxRadioBox(this, ID_Log_Avg, wxEmptyString, wxDefaultPosition, wxDefaultSize, avgChoices, 1 , wxRA_SPECIFY_ROWS), 1, wxEXPAND);

    auto* modular_sizer = new wxBoxSizer(wxHORIZONTAL);
    modular_sizer->Add(CreateCheckBox(this, ID_Log_ModularRAW, false, "RAW"));
    modular_sizer->Add(CreateCheckBox(this, ID_Log_ModularTIME, true, "time"));
    modular_sizer->Add(CreateCheckBox(this, ID_Log_ModularAVG, true, "avg"));
    modular_sizer->Add(CreateCheckBox(this, ID_Log_ModularDEV, true, "sd"));
    modular_sizer->Add(CreateCheckBox(this, ID_Log_ModularSEM, true, "dispersion"));

    auto logSettings = persistence.getLogSettings();

    freq_sizer->Add(CreateSizerWithTextAndSpin(this, ID_Log_AvgPeriod, "avg. period [ms] ", logSettings["averagePeriod"]), 1, wxEXPAND | wxTOP, 10);
    freq_sizer->Add(avg_sizer, 0, wxEXPAND);
    freq_sizer->Add(modular_sizer, 1, wxTOP, 20);

    auto* picker = new wxFilePickerCtrl(this, ID_Log_FilePicker,
                                        "./radarLog" + std::to_string(std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count()) + ".log",
                                        "log output file",
                                        wxFileSelectorDefaultWildcardStr,
                                        wxDefaultPosition,
                                        wxDefaultSize,
                                        wxFLP_SAVE | wxFLP_OVERWRITE_PROMPT | wxFLP_USE_TEXTCTRL);

    auto* startstopbtn = new wxToggleButton(this, ID_Log_StartStop, "START");

    control_sizer->Add(picker, 0, wxEXPAND);
	control_sizer->AddSpacer(10);
    control_sizer->Add(startstopbtn, 1, wxEXPAND);

    sizer->Add(freq_sizer, 1, wxALIGN_CENTER_VERTICAL | wxALL, 20);
    sizer->Add(control_sizer, 1, wxEXPAND | wxALL, 20);
    this->SetSizerAndFit(sizer);
}