/*
 * Copyright (c) 2013 Institute of Applied Physics of the Russian Academy of Sciences
 */

#ifndef MATH_BASICS_H_INCLUDED
#define MATH_BASICS_H_INCLUDED

#define DOUBLE_PRECISION    1e-12

#include <math.h>

inline double sqr(double a)
{
    return a*a;
}

inline bool isNearZero(double a)
{
    return fabs(a) < DOUBLE_PRECISION;
}


inline bool checkIfEqual(double a, double b)
{
    if (fabs(a-b) < DOUBLE_PRECISION) return true;
    return false;
}

inline double haversine(double angle)
{
    return sqr(sin(angle/2.0));
}

inline double inverseHaversine(double value)
{
    return 2.0*asin(sqrt(value));
}


double getArc(double angle1, double angle2);
double angleTo14Quadrant(double angle);

/**
 * @brief This function hides particularities of other functions.
 * @param centerValue   This value would be returned when x=0
 * @param radius        When |x| > radius return value is realValue
 * @param realValue     Value that we need to modificate
 * @param x             Position where we should get modificated (or non-modificated if not in radius) value
 */
double particularityHider(double centerValue, double radius, double realValue, double x);

#endif //MATH_BASICS_H_INCLUDED
