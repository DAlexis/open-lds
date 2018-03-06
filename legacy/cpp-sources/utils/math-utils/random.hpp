/*
 * Copyright (c) 2013 Institute of Applied Physics of the Russian Academy of Sciences
 */

#ifndef RANDOM_H_INCLUDED
#define RANDOM_H_INCLUDED

#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_real.hpp>
#include <boost/random/normal_distribution.hpp>
#include <boost/random/variate_generator.hpp>

class Random
{
public:
    static void randomizeUsingTime();
    static void randomize(unsigned int parameter);
    static double gaussian(double center, double dispersion);
    static double uniform(double from, double to);
private:
    static boost::mt19937 randomGenerator;
};


#endif // RANDOM_H_INCLUDED
