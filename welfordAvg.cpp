/*
 * microRadar software
 * (progetto polimi 2022)
 *
 * Salvatore Passaro
 */

#include "welfordAvg.h"

void welfordAvg::newValue(double v) {
    count++;
    double delta = v - m_avg;
    m_avg += delta/count;
    M2 += delta * (v - m_avg);

    max = std::max(v, max);
    min = std::min(v, min);
}

double welfordAvg::avg() {
    return count > 2 ? m_avg : 999;
}

double welfordAvg::var() {
    return count > 2 ? M2 / count : 999;
}

double welfordAvg::sd() {
    return count > 2 ? std::sqrt(var()) : 999;
}

double welfordAvg::mr() {
    return count > 2 ? (max - min) / 2.0 : 999;
}