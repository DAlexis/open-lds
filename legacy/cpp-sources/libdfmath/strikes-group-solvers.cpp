#include "strikes-group-solvers.hpp"
#include "exceptions.hpp"
#include <stdexcept>

#include <iostream>

StrikesGroupSolverBuilder* StrikesGroupSolverBuilder::m_solver = nullptr;
StrikesGroupSolverBuilder::StaticDeinitializer StrikesGroupSolverBuilder::deinitializer;

StrikesGroupSolverBuilder& StrikesGroupSolverBuilder::instance()
{
    if (!m_solver)
        m_solver = new StrikesGroupSolverBuilder;
    return *m_solver;
}

void StrikesGroupSolverBuilder::registerSolver(const std::string& solverName, IStrikesGroupSolverFactory* factory)
{
    std::cout << "Registering solver " << solverName << std::endl;
    auto it = m_factories.find(solverName);
    if (it != m_factories.end())
        throw std::logic_error(EX_PREFIX + "Solver \'" + solverName + "\' already registered.");
    
    m_factories[solverName] = factory;
}

IStrikesGroupSolver* StrikesGroupSolverBuilder::buildSolver(const std::string&& solverName)
{
    auto it = m_factories.find(solverName);
    if (it == m_factories.end())
        throw std::logic_error(EX_PREFIX + "Solver \'" + solverName + "\' does not exists!");
    
    return it->second->buildSolver();
}

StrikesGroupSolverBuilder::StaticDeinitializer::~StaticDeinitializer()
{
    if (StrikesGroupSolverBuilder::m_solver)
        delete StrikesGroupSolverBuilder::m_solver;
}
