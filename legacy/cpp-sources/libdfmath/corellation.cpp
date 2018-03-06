#include "corellation.hpp"
#include "strike-data.hpp"
#include "strikes-group.hpp"
#include "positioning.hpp"

#include <algorithm>

Corellation::Corellation(const StrikeData& original,
                    const StrikeData& shifted,
                    double timeShift)
{
    const double origStartT = std::max(original.getBufferBeginTimeShift(), shifted.getBufferBeginTimeShift() + timeShift);
    const double origStopT  = std::min(original.getBufferEndTimeShift(), shifted.getBufferEndTimeShift() + timeShift);
    
    originalEnergy = 0;
    shiftedEnergy = 0;
    corellationEnergy = 0;
    corellation = 0;
    
    if (origStartT > origStopT)
    {
        hasOverlapping = false;
        return;
    }
    hasOverlapping = true;
    
    const double dt = std::min(original.getTimeStep(), shifted.getTimeStep());
    
    for (double origT=origStartT; origT<=origStopT; origT+=dt)
    {
        double orB = original.getBByTimeShift(origT);
        double shB = shifted.getBByTimeShift(origT-timeShift);
        originalEnergy += sqr(orB) * dt;
        shiftedEnergy += sqr(shB) * dt;
        
        corellationEnergy += orB * shB * dt;
    }
    corellation = corellationEnergy / sqrt(originalEnergy * shiftedEnergy);
}

double getTimeShift(const StrikeData& first, const StrikeData& second, double precision)
{
    const double maxShift = first.getBufferEndTimeShift() - second.getBufferBeginTimeShift();
    const double minShift = first.getBufferBeginTimeShift() - second.getBufferEndTimeShift();
    const double dt = std::max(
			std::min(first.getTimeStep(), second.getTimeStep()),
			precision
	);
    
    double maxCorellation = -1.0;
    double bestTimeShift = 0;
    
    for (double timeShift = minShift*0.5; timeShift < maxShift*0.5; timeShift += dt)
    {
        Corellation corr(first, second, timeShift);
        if (not corr.hasOverlapping)
            continue;
        if (corr.corellation > maxCorellation)
        {
            maxCorellation = corr.corellation;
            bestTimeShift = timeShift;
        }
    }
    return bestTimeShift;
}
