/*
 * microRadar software
 * (progetto polimi 2022)
 *
 * Salvatore Passaro
 */

#ifndef MICRORADAR_MEASUREPANEL_H
#define MICRORADAR_MEASUREPANEL_H

#include "wx/wx.h"
#include "gui_helpers.h"
#include "mathplot.h"
#include <numeric>
#include <iomanip>
#include <sstream>

class measurePanel : public wxPanel{
    wxSizer *const sizer = new wxStaticBoxSizer(wxVERTICAL, this, "measure");
    wxStaticText* big_measure;
    mpFXYVector* line_graph;
    mpWindow* graph;
    std::deque<double> measure_history;

public:
    measurePanel(wxWindow* parent);
    void updateMeasure(float m);
    void clear();
};

#endif //MICRORADAR_MEASUREPANEL_H