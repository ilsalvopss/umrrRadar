/*
 * microRadar software
 * (progetto polimi 2022)
 *
 * Salvatore Passaro
 */

#include <cmath>
#include "triangularAvg.h"

void triangularAvg::newValue(double v) {
    count++;
    min = std::min(v, min);
    max = std::max(v, max);

    mode = (min + max) / 2;
}

double triangularAvg::avg() {
    return count > 2 ? ( min + max + mode ) / 3 : 999;
}

double triangularAvg::var() {
    return count > 2 ? ( std::pow(min, 2) + std::pow(max, 2) + std::pow(mode, 2) - min*max - min*mode - max*mode ) / 18 : 999;
}

double triangularAvg::sd() {
    return count > 2 ? std::sqrt(var()) : 999;
}

double triangularAvg::mr() {
    return count > 2 ? (max - min) / 2.0 : 999;
}