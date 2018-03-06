/*
 * Copyright (c) 2013-2014 Institute of Applied Physics of the Russian Academy of Sciences
 */

#include "lightning-positioning-result.hpp"

#include <iostream>
LightningPositioningResult::LightningPositioningResult():
    dispersion(0.0),
    worstCorellation(1),
    minimalDetFuncValue(0),
    isOnADetectionBorder(false),
    timeClusterId(-1),
    countOfDetections(0)
{
}

std::string LightningPositioningResult::getString()
{
    return "Pos: " + position.getString() + ", time: " + time.getString();
}

void LightningPositioningResult::writeSolution(MySQLConnectionManager& database) const
{
    MySQLInsertGenerator insert("solutions");
    
    int solutionStatusValue = isOnADetectionBorder ? SS_ON_THE_FAREST_BORDED : SS_GOOD;
    
    insert.set("round_time",          time.getRoundPart(), true);
    insert.set("fraction_time",       time.getFractionPart());
    insert.set("lon",                 position.getLongitude());
    insert.set("lat",                 position.getLatitude());
    insert.set("time_cluster",        timeClusterId);
    insert.set("status",              solutionStatusValue);
    insert.set("count_of_detections", countOfDetections);
    insert.set("dispersion",          dispersion);
    
    database << insert;
}
