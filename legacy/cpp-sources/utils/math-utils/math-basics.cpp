/*
 * Copyright (c) 2013 Institute of Applied Physics of the Russian Academy of Sciences
 */

#include "math-basics.hpp"
#include <math.h>

double getArc(double angle1, double angle2)
{
    double result = fabs(angle2 - angle1);
    if (result > M_PI)
        result = 2*M_PI - result;
    return result;
}


double angleTo14Quadrant(double angle)
{
    while (angle > M_PI/2.0)
        angle -= M_PI;
    
    while (angle < -M_PI/2.0)
        angle += M_PI;
    
    return angle;
}

double particularityHider(double centerValue, double radius, double realValue, double x)
{
    if (fabs(x) > radius)
        return realValue;
    double difference = realValue-centerValue;
    double weight = 0.5 - cos(x/radius*M_PI)/2.0;
    return centerValue + difference*weight;
}
