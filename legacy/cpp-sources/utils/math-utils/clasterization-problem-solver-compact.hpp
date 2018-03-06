/*
 * Copyright (c) 2014 Institute of Applied Physics of the Russian Academy of Sciences
 */

#ifndef CLASTERIZATION_PROBLEM_SOLVER_COMPACT_H_INCLUDED
#define CLASTERIZATION_PROBLEM_SOLVER_COMPACT_H_INCLUDED

#include "clasterization-problem-solver-interface.hpp"

#include <vector>
#include <list>

/**
 * @brief This clusterizer algorythm:
 *  1) Finding nearest clusters (by centers)
 *  2) Testing if can be merged -> merging
 *  3) goto 1
 */
template<class T>
class ClusterizationProblemSolverCompact : public IClusterizer<T>
{
public:
    class Cluster : public ICluster<T>
    {
        friend ClusterizationProblemSolverCompact;
    public:
        Cluster():
            m_weight(1), m_diameter(0)
        {}
        
        const T& center() const { return m_center; }
        double weight() const { return m_weight; }
        double diameter() const { return m_diameter; }
        
        /// Calculate maximal distance between 2 points in this cluster
        void calculateDiameter()
        {
            m_diameter = 0;
            for (auto it=m_elements.begin(); it != m_elements.end(); it++)
            {
                auto jt = it;
                for (jt++; jt != m_elements.end(); jt++)
                {
                    double dist = **jt - **it;
                    if (dist > m_diameter)
                        m_diameter = dist;
                }
            }
        }
        
        const std::list<T*>& elements() const { return m_elements; }
        
        /// Here we check that any point of candidate cluster is closer than distance to any point of this cluster
        bool isNear(double distance, const Cluster& candidate)
        {
            for (auto it=m_elements.begin(); it != m_elements.end(); it++)
                for (auto jt=candidate.m_elements.begin(); jt != candidate.m_elements.end(); jt++)
                    if (**jt - **it > distance)
                        return false;
            return true;
        }
        
        
    private:
        std::list<T*> m_elements;
        T m_center;
        double m_weight;
        double m_diameter;
        
    };
    
    ClusterizationProblemSolverCompact(CenterOfMassFunction<T> func) :
        m_pCMF(func)
    {}
    
    void add(const T& object)
    {
        m_objects.push_back(object);
        m_clusters.push_back(Cluster());
        m_clusters.back().m_elements.push_back(&m_objects.back());
        m_clusters.back().m_center = object;
    }
    
    const std::list<ICluster<T>*>& clusters() { return m_clusterInterfaces; }
    //const std::list<Cluster>& clusters() { return m_clusters; }
    
    void clusterize(double distance)
    {
        for (;;)
        {
            findNearestClusters();
            if (nearest1 == m_clusters.end()) // in case of clusters count is 1 etc.
                break;
            if (nearest1->isNear(distance, *nearest2))
                mergeNearest();
            else
                break;
        }
        m_clusterInterfaces.clear();
        for (auto it=m_clusters.begin(); it != m_clusters.end(); it++)
        {
            it->calculateDiameter();
            m_clusterInterfaces.push_back(static_cast<ICluster<T>*> (&(*it)) );
        }
    }
    
private:
    void findNearestClusters()
    {
        nearest2 = nearest1 = m_clusters.end();
        double distance = 0;
        for (auto it=m_clusters.begin(); it != m_clusters.end(); it++)
        {
            auto jt=it;
            for (jt++; jt != m_clusters.end(); jt++)
            {
                double newDist = jt->m_center-it->m_center;
                if (newDist < distance || nearest1 == m_clusters.end())
                {
                    nearest1 = it; nearest2 = jt;
                    distance = newDist;
                }
            }
        }
    }
    
    void mergeNearest()
    {
        nearest1->m_center = (*m_pCMF) (
                nearest1->m_center,
                nearest1->m_weight,
                nearest2->m_center,
                nearest2->m_weight
            );
        nearest1->m_weight += nearest2->m_weight;
        nearest1->m_elements.splice(nearest1->m_elements.end(), nearest2->m_elements);
        m_clusters.erase(nearest2);
        nearest2 = nearest1 = m_clusters.end();
    }
    
    CenterOfMassFunction<T> m_pCMF; // Center of Mass Function
    std::list<T> m_objects;
    std::list<Cluster> m_clusters;
    std::list<ICluster<T>*> m_clusterInterfaces;
    typename std::list<Cluster>::iterator nearest1, nearest2;
};


#endif // CLASTERIZATION_PROBLEM_SOLVER_COMPACT_H_INCLUDED
