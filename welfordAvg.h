/*
 * microRadar software
 * (progetto polimi 2022)
 *
 * Salvatore Passaro
 */

#ifndef MICRORADAR_WELFORDAVG_H
#define MICRORADAR_WELFORDAVG_H

#include "avgAccumulator.h"
#include <cmath>
#include <cfloat>
#include <algorithm>

//Welford's algorithm
//http://cpsc.yale.edu/sites/default/files/files/tr222.pdf
//https://en.wikipedia.org/wiki/Algorithms_for_calculating_variance#Welford's_online_algorithm
class welfordAvg : public avgAccumulator {
private:
    double m_avg = 0;
    double M2 = 0;
    unsigned int count = 0;
    double min = DBL_MAX;
    double max = 0;

public:
    void newValue(double v) override;
    double avg() override;
    double var() override;
    double sd() override;
    double mr() override;
};


#endif //MICRORADAR_WELFORDAVG_H