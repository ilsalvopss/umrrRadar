/*
 * microRadar software
 * (progetto polimi 2022)
 *
 * Salvatore Passaro
 */

#include "measurePanel.h"

void measurePanel::clear() {
    big_measure->SetLabel("0.00 m");

    measure_history.clear();
    line_graph->Clear();
}

/*
 * updates label and graph with a fresh measure
 * graph is not updated if the delta between current and last measure is too small
 */
void measurePanel::updateMeasure(float m){
    std::stringstream label;
    label << std::fixed << std::setprecision(2) << m;
    big_measure->SetLabel(label.str() + " m");

    if(!measure_history.empty() && std::abs(m - measure_history.back()) < .28)
        return;

    if(measure_history.size() > this->GetEffectiveMinSize().GetWidth())
        measure_history.pop_front();

    measure_history.push_back(m);

    std::vector<double> indexes(measure_history.size());
    for(int i = 0;i < measure_history.size(); i++)
        indexes[i] = i;

    line_graph->SetData(indexes, std::vector(measure_history.begin(), measure_history.end()));
    graph->UpdateAll();
    graph->Fit();
}

/*
 * constructs measurePanel and mpWindow GUI stuff
 */
measurePanel::measurePanel(wxWindow *parent) : wxPanel(parent) {
    auto* meter_sizer = new wxBoxSizer(wxVERTICAL);

    big_measure = new wxStaticText(this, wxID_ANY, "0.00 m");
    MakeControlFontMonospace(big_measure);
    SetControlFontSize(big_measure, 23.5);

    meter_sizer->Add(new wxStaticText(this, wxID_ANY, "last valid measure"), 1, wxALIGN_CENTER);
    meter_sizer->Add(big_measure, 1, wxALIGN_CENTER);

    graph = new mpWindow(this, wxID_ANY);
    graph->SetBackgroundColour(this->GetBackgroundColour());

    line_graph = new mpFXYVector();
    line_graph->SetContinuity(true);
    line_graph->SetPen(wxPen(wxColour(0,170,255),3));

    graph->AddLayer(line_graph, true);
    line_graph->SetVisible(true);

    sizer->Add(meter_sizer, 1, wxEXPAND);
    sizer->Add(graph, 1, wxEXPAND);

    graph->Fit();
    graph->EnableMousePanZoom(false);

    this->SetSizerAndFit(sizer);
}