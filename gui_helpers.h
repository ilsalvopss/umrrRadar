/*
 * microRadar software
 * (progetto polimi 2022)
 *
 * Salvatore Passaro
 */

#ifndef MICRORADAR_GUI_HELPERS_H
#define MICRORADAR_GUI_HELPERS_H

#include <wx/wx.h>
#include <wx/tglbtn.h>
#include <wx/spinctrl.h>
#include <codecvt> // codecvt_utf8
#include <locale>  // wstring_convert
#include <unordered_map>
#include "welfordAvg.h"
#include "triangularAvg.h"

wxDEFINE_EVENT(ERROR_EVENT, wxCommandEvent);

#define AUTO_TRIGGERED  69

enum  //mainFrame enum, here for ease of use
{
    ID_Settings = 1,
    ID_Conn_Auto_ToggleConn = 66,
    ID_Conn_Auti_ToggleLog,
    ID_Conn_COMOpenClose,
    ID_Conn_BaudChoice,
    ID_Conn_COMCombobox,
    ID_Log_StartStop,
    ID_Callback_Error,
    ID_Log_AvgPeriod,
    ID_Log_FilePicker,
    ID_Log_ModularTIME,
    ID_Log_ModularAVG,
    ID_Log_ModularDEV,
    ID_Log_ModularSEM,
    ID_Log_Avg
};

enum ErrorID{
    Phy_Failed,
    FS_Failed
};

enum modularFlags{
    NONE = 0,
    TIME = 1 << 0,
    AVG = 1 << 1,
    SEM = 1 << 2,
    DEV = 1 << 3
};

const std::unordered_map<std::string, std::function<std::unique_ptr<avgAccumulator>()>> avgAccumulators = {
        {"welford", []() { return std::make_unique<welfordAvg>(); } },
        {"triangular", []() { return std::make_unique<triangularAvg>(); } }
};

const std::vector<int> BAUD_RATES = {9600, 14400, 19200, 38400, 57600, 115200, 128000, 256000};

std::string to_narrow(std::wstring& wide_string);
std::wstring to_wide(std::string& str);

wxSizer *CreateSizerWithTextAndToggleButton(wxWindow* parent, wxWindowID btnId, const char* label, bool btnInitialState);
wxSizer *CreateSizerWithTextAndDropdown(wxWindow* parent, wxChoice* choice, const char* label, bool smaller = false);
wxSizer *CreateSizerWithTextAndCombobox(wxWindow* parent, wxComboBox* comboBox, const char* label);
wxSizer *CreateSizerWithTextAndSpin(wxWindow* parent, wxWindowID spinId, const char* label, int initial_value, bool smaller = false);

wxCheckBox* CreateCheckBox(wxWindow* parent, wxWindowID id, bool state, const char* label);
wxTextCtrl* makeTextCtrlMinSize(wxTextCtrl*, int len);

void PopulateItemContainerWithIntVector(wxItemContainer* c, const std::vector<int>& v);

void MakeControlFontSmaller(wxControl* ctrl);
void MakeControlFontBold(wxControl* ctrl);
void MakeControlFontMonospace(wxControl* ctrl);
void SetControlFontSize(wxControl* ctrl, double size);

#endif //MICRORADAR_GUI_HELPERS_H