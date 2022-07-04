/*
 * microRadar software
 * (progetto polimi 2022)
 *
 * Salvatore Passaro
 */

#ifndef MICRORADAR_TRIANGULARAVG_H
#define MICRORADAR_TRIANGULARAVG_H

#include <cfloat>
#include <algorithm>
#include "avgAccumulator.h"

class triangularAvg : public avgAccumulator{
    double min = DBL_MAX;
    double max = 0;
    double mode = 0;

    unsigned int count = 0;

public:
    void newValue(double v) override;
    double avg() override;
    double var() override;
    double sd() override;
    double mr() override;
};


#endif //MICRORADAR_TRIANGULARAVG_H
