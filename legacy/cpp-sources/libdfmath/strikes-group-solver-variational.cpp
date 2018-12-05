#include "strikes-group-solver-variational.hpp"
#include "math-utils.hpp"
#include "corellation.hpp"

#include "constants.hpp"
#include "exceptions.hpp"

#include <functional>
#include <iostream>
#include <thread>
#include <mutex>

volatile StrikesGroupVariationalSolverFactory::StrikesGroupVariationalSolverFactoryStaticInitializer StrikesGroupVariationalSolverFactory::m_initializer;
StrikesGroupVariationalSolverFactory* StrikesGroupVariationalSolverFactory::m_factory = nullptr;


StrikesGroupVariationalSolverFactory::StrikesGroupVariationalSolverFactoryStaticInitializer
    ::StrikesGroupVariationalSolverFactoryStaticInitializer()
{
    StrikesGroupVariationalSolverFactory::m_factory = new StrikesGroupVariationalSolverFactory;
    StrikesGroupSolverBuilder::instance().registerSolver("variational-2d", m_factory);
}

StrikesGroupVariationalSolverFactory::StrikesGroupVariationalSolverFactoryStaticInitializer
    ::~StrikesGroupVariationalSolverFactoryStaticInitializer()
{
    if (StrikesGroupVariationalSolverFactory::m_factory)
        delete StrikesGroupVariationalSolverFactory::m_factory;
}

///////////////////////////////////
// StrikesGroupVariationalSolverFactory


StrikesGroupVariationalSolverFactory::StrikesGroupVariationalSolverFactory()
{
}


IStrikesGroupSolver* StrikesGroupVariationalSolverFactory::buildSolver()
{
    return new StrikesGroupVariationalSolver;
}

///////////////////////////////////
// StrikesGroupVariationalSolver

StrikesGroupVariationalSolver::~StrikesGroupVariationalSolver()
{
    reset();
}

void StrikesGroupVariationalSolver::solve(StrikesGroup& group)
{
    m_group = &group;
}

void StrikesGroupVariationalSolver::reset() noexcept
{
    m_group = nullptr;
    m_isSolved = false;
    m_cachedPositions.reset();
    m_cachedTimes.reset();
    m_strikesCount = 0;
}

const std::vector<LightningPositioningResult>& StrikesGroupVariationalSolver::solutions()
{
    if (!m_group)
        throw std::logic_error(EX_PREFIX + "Strikes group not set for solver!");
    if (lightnings.empty())
        solveLightningPositioningProblem();
    return lightnings;
}

LightningPositioningResult& StrikesGroupVariationalSolver::getMostProbableSolution()
{
    if (!m_group)
        throw std::logic_error(EX_PREFIX + "Strikes group not set for solver!");
    
    if (lightnings.empty())
        solveLightningPositioningProblem();
    return lightnings.front();
}

bool StrikesGroupVariationalSolver::isEnoughData()
{
    if (m_group->strikes.size() == 0)
        return false;
    if (m_group->strikes.size() < 3 && !Configuration::Instance().solving.useDirectionFinding)
        return false;
    return true;
}

void StrikesGroupVariationalSolver::solveLightningPositioningProblem()
{
    if (m_group->getTimeClusterStatus() != StrikesGroup::CS_ONE_CLUSTER)
        throw std::logic_error(EX_PREFIX + "Can solve only one-cluster group");
    
    lightnings.clear();    
    
    Function2D detFunction = std::bind(
        static_cast<double (StrikesGroupVariationalSolver::*)(double, double) const>(
            &StrikesGroupVariationalSolver::calculateDetectionFunction
        ),
        this,
        std::placeholders::_1,
        std::placeholders::_2
    );
    
    std::unique_ptr<IClusterizer<Position>> pClusterizer(ClusterizatorsBuilder<Position>().createClusterizer("compact", calculateCenterOfMass));
    //ClasterizationProblemSolver<Position> clusterisator;
    //clusterisator.setCenterMassFunc(&calculateCenterOfMass);
    
    const unsigned int initPointsGridLinesCount = Configuration::Instance().solving.initPointsGridLinesCount;
    
    // Here we think that points are not enough one North/South poles
    const double latLonMargins = Configuration::Instance().solving.latLonMargins;
    const double latMin = m_group->getMinLat() - latLonMargins;
    const double latMax = m_group->getMaxLat() + latLonMargins;
    const double lonMin = m_group->getMinLon() - latLonMargins;
    const double lonMax = m_group->getMaxLon() + latLonMargins;
    
    doPreSolvingJobs();
    
    std::vector<Position> initialPoints;
    for (unsigned int i = 0; i<initPointsGridLinesCount; i++)
    {
        for (unsigned int j = 0; j<initPointsGridLinesCount; j++)
        {
            initialPoints.push_back(
                Position(latMin + (latMax-latMin)*i/(initPointsGridLinesCount-1),
                         lonMin + (lonMax-lonMin)*j/(initPointsGridLinesCount-1))
            );
        }
    }
    
    /// @TODO Add std::unique_lock instead of calling lock/unlock
    std::mutex clasterisatorAdding;
    
    unsigned int threadsCount = Configuration::Instance().solving.threadsCount;
    
    std::vector<std::thread> workers;
    double currentIndex = 0;
    double indexIncrement = double(initialPoints.size()) / double(threadsCount);
    for (unsigned int t=0; t<threadsCount; t++)
    {
        unsigned int beginIndex = round(currentIndex);
        unsigned int endIndex = round(currentIndex + indexIncrement);
        workers.push_back(std::thread(
                [&clasterisatorAdding, &detFunction, &initialPoints, &pClusterizer]
                    (unsigned int beginIndex, unsigned int endIndex) {
                    
                    for (unsigned int i=beginIndex; i<endIndex; i++)
                    {
                        GradientDescentSolver solver;
                        solver.setLimitsUsage(false);
                        solver.setFunction(detFunction);
                        solver.setInitialStep(Configuration::Instance().solving.GDSInitialStep);
                        solver.setXYPrecision(Configuration::Instance().solving.GDSPrecision);
                        solver.setStartPosition(initialPoints[i].getLatitude(),
                                                initialPoints[i].getLongitude());
                        solver.runVariableStep();
                        
                        clasterisatorAdding.lock();
                            pClusterizer->add(
                                Position(solver.getX(),
                                         solver.getY())
                            );
                            
                        clasterisatorAdding.unlock();
                    }
                    
                },
                beginIndex,
                endIndex
            )
        );
        
        currentIndex += indexIncrement;
    }
    
    for (auto it=workers.begin(); it!=workers.end(); it++)
        it->join();
    
    //clusterisator.findClusters(Configuration::Instance().solving.clusterRadius);
    pClusterizer->clusterize(Configuration::Instance().solving.clusterRadius);
    
    for (auto it = pClusterizer->clusters().begin(); it != pClusterizer->clusters().end(); it++)
    {
        lightnings.push_back(LightningPositioningResult());
        LightningPositioningResult &added = lightnings.back();
        added.timeClusterId = m_group->getTimeClusterId();
        added.position = (*it)->center();
        added.isOnADetectionBorder = isOnADetectionBorder(added.position);
        added.countOfDetections = m_group->size();
        
        // Calculating time shift
        double averageTimeShift = 0;
        for (auto it = m_group->begin(); it != m_group->end(); it++)
        {
            averageTimeShift += (it->time - m_group->front().time) - (it->position - added.position)/Global::LightSpeed;
        }
        averageTimeShift /= m_group->size();
        
        added.time = m_group->front().time + averageTimeShift;
        
        // Calculating min value of det function on this cluster
        MinMaxFinder<double> detFuncValMinMax;
        
        for (auto pPoint = (*it)->elements().begin(); pPoint != (*it)->elements().end(); pPoint++)
            detFuncValMinMax.add(calculateDetectionFunction(**pPoint));
        
        added.minimalDetFuncValue = detFuncValMinMax.getMin();
        
        added.dispersion = (*it)->diameter();
    }
    
    std::sort(
        lightnings.begin(),
        lightnings.end(),
        [](const LightningPositioningResult& p1, const LightningPositioningResult& p2)
        {
            return p1.minimalDetFuncValue < p2.minimalDetFuncValue;
        }
    );
    
    corellationTable.clear();
}

void StrikesGroupVariationalSolver::buildCorellationTable()
{
    // Hm. Initializing 2d array
    corellationTable.clear();
    corellationTable.reserve(m_group->size());
    for (size_t i=0; i<m_group->size(); i++)
    {
        corellationTable.push_back(std::vector<double>(m_group->size()));
    }
    
    for (size_t i=0; i<m_group->size(); i++)
        for (size_t j=0; j<m_group->size(); j++)
        {
            double timeShift = getTimeShift((*m_group)[i], (*m_group)[j]);
            corellationTable[i][j] = timeShift;
            corellationTable[j][i] = -timeShift;
        }
}

double StrikesGroupVariationalSolver::calculateTimeOfArrivalDetFunc(const Position& location) const
{
    
    if (!Configuration::Instance().solving.useTimeOfArrival)
        return 0.0;
        
    /// Warning! Here commented SLOW version of this fuinction. So it can discover some ways to optimize calculations
    
    /*
    double result=0.0;
    // For debug purpose
    //int q=0;
    for (size_t i = 0; i < m_group->size(); i++)
    {
        for (auto j = i+1; j < m_group->size(); j++)
        {   
            // If data is obviously incorrect (if all works normally, there is no need in this check)
            if (((*m_group)[i].time - (*m_group)[j].time) > ((*m_group)[i].position - (*m_group)[j].position) / Global::LightSpeed + Configuration::Instance().precision.timePrecision ) continue;
            
            double theoryTime = ((location - (*m_group)[i].position) - (location - (*m_group)[j].position)) / Global::LightSpeed;
            double experimentTime = (*m_group)[i].time - (*m_group)[j].time + corellationTable[j][i];
            double difference = fabs(theoryTime - experimentTime);
            
            // For debug purpose
            //if (q++ == 0)
            result += 10*sqr(difference);
        }
    }
    result /= sqr(Configuration::Instance().boltek.maxDetectionDistance / Global::LightSpeed);
    return result;
    */
    
    /*
    double result=0.0;
    // For debug purpose
    //int q=0;
    const std::vector<StrikeData> &strikes = m_group->getStrikes();
    for (size_t i = 0; i < m_group->size(); i++)
    {
        for (auto j = i+1; j < m_group->size(); j++)
        {   
            // If data is obviously incorrect (if all works normally, there is no need in this check)
            //if ((strikes[i].time - strikes[j].time) > (strikes[i].position - strikes[j].position) / Global::LightSpeed + Configuration::Instance().precision.timePrecision ) continue;
            
            double theoryTime = ((location - strikes[i].position) - (location - strikes[j].position)) / Global::LightSpeed;
            double experimentTime = strikes[i].time - strikes[j].time + corellationTable[j][i];
            double difference = fabs(theoryTime - experimentTime);
            
            // For debug purpose
            //if (q++ == 0)
            result += 10*sqr(difference);
        }
    }
    result /= sqr(Configuration::Instance().boltek.maxDetectionDistance / Global::LightSpeed);
    return result;
    */
    
    
    double result=0.0;
    // For debug purpose
    //int q=0;
    for (size_t i = 0; i < m_strikesCount; i++)
    {
        for (auto j = i+1; j < m_strikesCount; j++)
        {
            // If data is obviously incorrect (if all works normally, there is no need in this check)
            //if ((strikes[i].time - strikes[j].time) > (strikes[i].position - strikes[j].position) / Global::LightSpeed + Configuration::Instance().precision.timePrecision ) continue;
            
            double theoryTime = ((location - m_cachedPositions[i]) - (location - m_cachedPositions[j])) / Global::LightSpeed;
            double experimentTime = m_cachedTimes[i] - m_cachedTimes[j] + corellationTable[j][i];
            double difference = fabs(theoryTime - experimentTime);
            
            // For debug purpose
            //if (q++ == 0)
            result += 10*sqr(difference);
        }
    }
    result /= sqr(Configuration::Instance().boltek.maxDetectionDistance / Global::LightSpeed);
    return result;
    
    
    //return m_group->calculateTimeOfArrivalDetFunc(location);
}

double StrikesGroupVariationalSolver::calculateDirectionFindingDetFunc(const Position& location) const
{
    if (!Configuration::Instance().solving.useDirectionFinding)
        return 0.0;
    /// @TODO Add azimuths cache!
    double result = 0.0;
    for (auto it = m_group->begin(); it != m_group->end(); it++)
    {
        double mesuredAzimuth = it->getDirection().azimuth;
        double diffAngle = angleTo14Quadrant(it->position.getAzimuth(location)-mesuredAzimuth);
        
        result += particularityHider(/*sqr(M_PI_2)*/0.0, NEAREST_ZONE, sqr(diffAngle), location-it->position) / it->getDirection().errorRate;
    }
    result /= sqr(M_PI);
    return result;
}

double StrikesGroupVariationalSolver::calculateFiniteRadiusTerms(const Position& location) const
{
    double sum = 0;
    for (auto it = m_group->begin(); it != m_group->end(); it++)
    {
        sum += it->calculateFiniteRadiusTerm(location);
    }
    return sum;
}

bool StrikesGroupVariationalSolver::isOnADetectionBorder(const Position& location) const
{
    return (calculateFiniteRadiusTerms(location) != 0);
}

double StrikesGroupVariationalSolver::calculateDetectionFunction(const Position& location) const
{
    return  Configuration::Instance().solving.timeOfArrivalWeight       * calculateTimeOfArrivalDetFunc(location)+
            Configuration::Instance().solving.directionFindingWeight    * calculateDirectionFindingDetFunc(location)+
            Configuration::Instance().solving.finiteRadiusTermWeight    * calculateFiniteRadiusTerms(location);
}

double StrikesGroupVariationalSolver::calculateDetectionFunction(double latitude, double longitude) const
{
    return calculateDetectionFunction(Position(latitude, longitude));
}

void StrikesGroupVariationalSolver::doPreSolvingJobs()
{
    buildCorellationTable();
    m_strikesCount = m_group->size();
    m_cachedPositions.reset(new Position[m_strikesCount]);
    m_cachedTimes.reset(new Time[m_strikesCount]);
    size_t i=0;
    for (auto it = m_group->begin(); it != m_group->end(); it++, i++)
    {
        it->findDirection();
        m_cachedPositions[i] = it->position;
        m_cachedTimes[i] = it->time;
    }
}
