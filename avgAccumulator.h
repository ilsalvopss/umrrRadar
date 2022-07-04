/*
 * microRadar software
 * (progetto polimi 2022)
 *
 * Salvatore Passaro
 */

#ifndef MICRORADAR_AVGACCUMULATOR_H
#define MICRORADAR_AVGACCUMULATOR_H

// base abstract class for accumulators implementations
class avgAccumulator {
public:
    virtual ~avgAccumulator() = default;

    virtual void newValue(double v) = 0;

    // average
    virtual double avg() = 0;

    // variance
    virtual double var() = 0;

    // standard deviation
    virtual double sd() = 0;

    // mid range (semidispersione massima)
    virtual double mr() = 0;
};


#endif //MICRORADAR_AVGACCUMULATOR_H