/*
 * Copyright (c) 2013-2014 Institute of Applied Physics of the Russian Academy of Sciences
 */

#ifndef LIGHTNING_POSITIONING_RESULT_HPP_INCLUDED
#define LIGHTNING_POSITIONING_RESULT_HPP_INCLUDED

#include "strike-data.hpp"

class LightningPositioningResult
{
public:
    LightningPositioningResult();
    
    Position position;
    Time time;
    double dispersion = 0;
    
    /// Worst corellation between registered B fields
    double worstCorellation = 0;
    
    /// Minimal value of detection function. This field is only for sorting potential solutions by it's probability
    double minimalDetFuncValue = 0;
    
    /// This flag is true if strike detected on a farest border of space where it could be detected
    bool isOnADetectionBorder = false;
    
    /// Get human-readable string for solution
    std::string getString();
    
    /// Id of time cluster
    size_t timeClusterId = 0;
    
    /// Count of stations detected this strike
    unsigned int countOfDetections = 0;
    
    /// Write this solution to database
    void writeSolution(MySQLConnectionManager& database) const;
    
private:
    std::map<std::string, std::string> m_fields;
};

#endif // LIGHTNING_POSITIONING_RESULT_HPP_INCLUDED
