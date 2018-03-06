#ifndef CORELLATION_H_INCLUDED
#define CORELLATION_H_INCLUDED

class StrikeData;

/** @brief This class calculates corellation function for two strikes. All work is in constructor.
 * Important: corellation is calculated for B-field buffers only! time field is not used.
 */ 
class Corellation
{
public:
    Corellation(const StrikeData& original,
                        const StrikeData& shifted,
                        double timeShift);
    
    /// True if signals has overlapping with givet timeShift
    bool hasOverlapping;
    /// Energy of original signal OVER SIGNALS OVERLAPPING only (not full original's energy!)
    double originalEnergy;
    /// Energy of shifted signal OVER SIGNALS OVERLAPPING only (not full shifted's energy!)
    double shiftedEnergy;
    /// Mutual energy over signals overlapping
    double corellationEnergy;
    /// corellationEnergy normad by originalEnergy and shiftedEnergy
    double corellation;
};

double getTimeShift(const StrikeData& first, const StrikeData& second, double precision = 0);

#endif // CORELLATION_H_INCLUDED 
