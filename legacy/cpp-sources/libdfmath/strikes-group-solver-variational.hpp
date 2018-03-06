#ifndef STRIKES_GROUP_SOLVER_VARIATIONAL_HPP_INCLUDED
#define STRIKES_GROUP_SOLVER_VARIATIONAL_HPP_INCLUDED

#include "strikes-group-solvers.hpp"
#include <boost/shared_array.hpp>

class StrikesGroupVariationalSolver : public IStrikesGroupSolver
{
public:
    void solve(StrikesGroup& group);
    void reset() noexcept;
    const std::vector<LightningPositioningResult>& solutions();
    LightningPositioningResult& getMostProbableSolution();
    
    ~StrikesGroupVariationalSolver();
    
private:
    bool isEnoughData();
    void solveLightningPositioningProblem();
    
    /** @brief         Calculate function that should be minimum in a strike place
     * @param location Point where ve want to calculate function
     * @return         Function's value in sec^2
     */
    double calculateDetectionFunction(const Position& location) const;
    
    /// The same as another variant of this function, but with latitude and longitude as double
    double calculateDetectionFunction(double latitude, double longitude) const;
    
    /// Time-of-arrival part of detection function
    double calculateTimeOfArrivalDetFunc(const Position& location) const;
    
    /// Direction finding part of detection function
    double calculateDirectionFindingDetFunc(const Position& location) const;
    
    /// Returns sum of finite radius terms for all strikes
    double calculateFiniteRadiusTerms(const Position& location) const;
    
    /// Check of position located on a detection border of any device registered it
    bool isOnADetectionBorder(const Position& location) const;
    
    /** @brief This function fullfills corellationTable
     */
    void buildCorellationTable();
    
    /** @brief Do all job that should be done before solving: building correlation table, finding directions etc.
     */
    void doPreSolvingJobs();
    
    StrikesGroup* m_group = nullptr;
    bool m_isSolved = false;
    
    std::vector< LightningPositioningResult > lightnings;
    std::vector<std::vector<double>> corellationTable;
    
    boost::shared_array<Position> m_cachedPositions;
    boost::shared_array<Time> m_cachedTimes;
    size_t m_strikesCount = 0;
};

class StrikesGroupVariationalSolverFactory : public IStrikesGroupSolverFactory
{
public:
    IStrikesGroupSolver* buildSolver();

    StrikesGroupVariationalSolverFactory();
private:
    
    
    struct StrikesGroupVariationalSolverFactoryStaticInitializer
    {
        StrikesGroupVariationalSolverFactoryStaticInitializer();
        ~StrikesGroupVariationalSolverFactoryStaticInitializer();
    };
    
    static volatile StrikesGroupVariationalSolverFactoryStaticInitializer m_initializer;
    static StrikesGroupVariationalSolverFactory *m_factory;
};

#endif // STRIKES_GROUP_SOLVER_VARIATIONAL_HPP_INCLUDED
