/*
 * Copyright (c) 2013 Institute of Applied Physics of the Russian Academy of Sciences
 */

#include "random.hpp"
#include "math-basics.hpp"

//////////////////////
// Random generator

boost::mt19937 Random::randomGenerator;


void Random::randomize(unsigned int parameter)
{
    randomGenerator.seed(parameter);
}

void Random::randomizeUsingTime()
{
    randomize(static_cast<unsigned int>(time(0)));
}

double Random::gaussian(double center, double dispersion)
{
    if (dispersion < DOUBLE_PRECISION)
        return center;
    boost::normal_distribution<> distr(center, dispersion);
    boost::variate_generator<boost::mt19937&, boost::normal_distribution<> > varGen(randomGenerator, distr);
    return varGen();
}

double Random::uniform(double from, double to)
{
    boost::uniform_real<> distr(from, to);
    boost::variate_generator<boost::mt19937&, boost::uniform_real<> > varGen(randomGenerator, distr);
    return varGen();
}

