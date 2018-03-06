/*
 * Copyright (c) 2014 Institute of Applied Physics of the Russian Academy of Sciences
 */

#ifndef CLASTERIZATION_PROBLEM_SOLVER_INTERFACE_H_INCLUDED
#define CLASTERIZATION_PROBLEM_SOLVER_INTERFACE_H_INCLUDED

#include <string>
#include <list>

template<typename T>
using CenterOfMassFunction = T (*)(const T& obj1, double weight1, const T& obj2, double weight2);

template<class T>
class ICluster
{
public:
    virtual const T& center() const = 0;
    virtual double weight() const = 0;
    virtual double diameter() const = 0;
    virtual const std::list<T*>& elements() const = 0;
    virtual ~ICluster() {}
};

template<class T>
class IClusterizer
{
public:
    virtual void add(const T& object) = 0;
    virtual void clusterize(double distance) = 0;
    virtual const std::list<ICluster<T>*>& clusters() = 0;
    virtual ~IClusterizer() {}
};

template<class T>
class IClusterizerBuilder
{
public:
    virtual IClusterizer<T>* createClusterizer(const std::string& type, CenterOfMassFunction<T> func) = 0;
    virtual ~IClusterizerBuilder() {}
};

#endif // CLASTERIZATION_PROBLEM_SOLVER_INTERFACE_H_INCLUDED

