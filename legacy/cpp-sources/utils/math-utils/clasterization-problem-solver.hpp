/*
 * Copyright (c) 2013-2014 Institute of Applied Physics of the Russian Academy of Sciences
 */

#ifndef CLASTERIZATION_PROBLEM_SOLVER_H_INCLUDED
#define CLASTERIZATION_PROBLEM_SOLVER_H_INCLUDED

#include <stdexcept>

#include "clasterization-problem-solver-interface.hpp"
#include "clasterization-problem-solver-compact.hpp"
#include "clasterization-problem-solver-compact-quick.hpp"

template<class T>
class ClusterizatorsBuilder : public IClusterizerBuilder<T>
{
public:
    IClusterizer<T>* createClusterizer(const std::string& type, CenterOfMassFunction<T> func)
    {
        if (type == "compact")
            return new ClusterizationProblemSolverCompact<T>(func);
        if (type == "compact-quick")
            return new ClusterizationProblemSolverCompactQuick<T>(func);
        throw std::invalid_argument(std::string("Clusterizer with type ") + type + " does not exist");
    }
};

#endif //CLASTERIZATION_PROBLEM_SOLVER_H_INCLUDED
