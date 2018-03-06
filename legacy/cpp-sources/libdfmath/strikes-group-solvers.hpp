#ifndef STRIKES_GROUP_SOLVERS_HPP_INCLUDED
#define STRIKES_GROUP_SOLVERS_HPP_INCLUDED

#include "strikes-group.hpp"
#include <map>
#include <string>

class IStrikesGroupSolver
{
public:
    virtual ~IStrikesGroupSolver() {}
    virtual void solve(StrikesGroup&) = 0;
    virtual const std::vector<LightningPositioningResult>& solutions() = 0;
    virtual void reset() = 0;
    virtual LightningPositioningResult& getMostProbableSolution() = 0;
};

class IStrikesGroupSolverFactory
{
public:
    virtual ~IStrikesGroupSolverFactory() {}
    virtual IStrikesGroupSolver* buildSolver() = 0;
};

class StrikesGroupSolverBuilder
{
public:
    static StrikesGroupSolverBuilder& instance();
    void registerSolver(const std::string& solverName, IStrikesGroupSolverFactory* factory);
    IStrikesGroupSolver* buildSolver(const std::string&& solverName);
    
private:
    struct StaticDeinitializer
    {
        ~StaticDeinitializer();
    };
    static StaticDeinitializer deinitializer;
    static StrikesGroupSolverBuilder* m_solver;
    std::map<std::string, IStrikesGroupSolverFactory*> m_factories;
};

#endif // STRIKES_GROUP_SOLVERS_HPP_INCLUDED
