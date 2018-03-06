/*
 * Copyright (c) 2014 Institute of Applied Physics of the Russian Academy of Sciences
 */

#ifndef CLASTERIZATION_PROBLEM_SOLVER_COMPACT_QUICK_H_INCLUDED
#define CLASTERIZATION_PROBLEM_SOLVER_COMPACT_QUICK_H_INCLUDED

#include "clasterization-problem-solver-interface.hpp"

#include <vector>
#include <list>

/**
 * @brief This clusterizer algorythm:
 *  1) Putting first point to first cluster
 *  2) Filling this cluster (testing every other point to be near)
 *  3) Putting first non-clusterized point to second cluster
 *  4) Filling second cluster
 *  5) ...
 */

template<class T>
class ClusterizationProblemSolverCompactQuick : public IClusterizer<T>
{
public:
    class Cluster : public ICluster<T>
    {
        friend ClusterizationProblemSolverCompactQuick;
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
    
    ClusterizationProblemSolverCompactQuick(CenterOfMassFunction<T> func) :
        m_pCMF(func)
    {}
    
    void add(const T& object)
    {
        m_objects.push_back(object);
        m_clusters.push_back(new Cluster());
        m_clusters.back()->m_elements.push_back(&m_objects.back());
        m_clusters.back()->m_center = object;
    }
    
    const std::list<ICluster<T>*>& clusters() { return m_clusterInterfaces; }
    
    void clusterize(double distance)
    {
        for (unsigned int currentIndex=0; currentIndex < m_clusters.size(); currentIndex++)
            for (unsigned int i=currentIndex+1; i < m_clusters.size();)
                if (m_clusters[currentIndex]->isNear(distance, *m_clusters[i]))
                    merge(currentIndex, i);
                else
                    i++;
        
        m_clusterInterfaces.clear();
        for (auto it=m_clusters.begin(); it != m_clusters.end(); it++)
        {
            (*it)->calculateDiameter();
            m_clusterInterfaces.push_back(static_cast<ICluster<T>*> (*it) );
        }
    }
    
    ~ClusterizationProblemSolverCompactQuick()
    {
        for (auto it = m_clusters.begin(); it != m_clusters.end(); it++)
            delete *it;
    }
    
private:
    
    void merge(unsigned int c1, unsigned int  c2)
    {
        m_clusters[c1]->m_center = (*m_pCMF) (
                m_clusters[c1]->m_center,
                m_clusters[c1]->m_weight,
                m_clusters[c2]->m_center,
                m_clusters[c2]->m_weight
            );
        m_clusters[c1]->m_weight += m_clusters[c2]->m_weight;
        m_clusters[c1]->m_elements.splice(m_clusters[c1]->m_elements.end(), m_clusters[c2]->m_elements);
        delete m_clusters[c2];
        m_clusters[c2] = m_clusters.back();
        m_clusters.pop_back();
    }
    
    CenterOfMassFunction<T> m_pCMF; // Center of Mass Function
    std::list<T> m_objects;
    std::vector<Cluster*> m_clusters;
    std::list<ICluster<T>*> m_clusterInterfaces;
    typename std::list<Cluster>::iterator nearest1, nearest2;
};


#endif // CLASTERIZATION_PROBLEM_SOLVER_COMPACT_QUICK_H_INCLUDED
