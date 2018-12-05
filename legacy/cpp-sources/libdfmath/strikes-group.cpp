/*
 * Copyright (c) 2013 Institute of Applied Physics of the Russian Academy of Sciences
 */

#include "strikes-group.hpp"
#include "constants.hpp"
#include "corellation.hpp"
#include "math-utils.hpp"

#include <limits>
#include <iomanip>
#include <algorithm>
#include <boost/algorithm/string.hpp>
#include <boost/bind.hpp>
#include <thread>
#include <mutex>
#include <iostream>

StrikesGroup::StrikesGroup() :
    timeClusterStatus(CS_EMPTY),
    timeClusterId(0)
{
}

StrikesGroup::~StrikesGroup()
{
}

size_t StrikesGroup::insertStrike(StrikeData& strike)
{
    strikes.push_back(strike);
    testMinMax(strikes.size() - 1);
    timeClusterStatus = CS_UNDEFINED;
    return strikes.size()-1;
}

void StrikesGroup::clear()
{
    strikes.clear();
}


void StrikesGroup::testMinMax(size_t index)
{
    strikeTimeMinMax.add(strikes[index].time, index);
    latitudeMinMax.add(strikes[index].position.getLatitude(), index);
    longitudeMinMax.add(strikes[index].position.getLongitude(), index);
}

void StrikesGroup::minMaxTestRange(size_t from, size_t to)
{
    for (size_t i = from; i < to; i++)
        testMinMax(i);
}

void StrikesGroup::findAllMinMax()
{
    strikeTimeMinMax.clear();
    latitudeMinMax.clear();
    longitudeMinMax.clear();
    minMaxTestRange(0, strikes.size());
}

int StrikesGroup::loadStrikes(TableType tableType, MySQLConnectionManager& mysql)
{
    timeClusterStatus = CS_UNDEFINED;
    int count = 0;
    do
    {
        StrikeData strike;
        strike.read(tableType, mysql);
        insertStrike(strike);
        count++;
        //std::cout << "s ";
    } while (mysql.nextRow());
    
    return count;
}

size_t StrikesGroup::readBoltekStrikesWithIdsInRange(MySQLConnectionManager& mysql, size_t firstId, size_t lastId)
{
    std::ostringstream query;
    query << "SELECT * FROM strikes WHERE ((global_id >= " << firstId << ") && (global_id <= " << lastId << "));";
    
    if (not mysql.query(query.str()))
        return 0;
    size_t count = 0;
    do
    {
        try {
            StrikeData strike;
            strike.read(TT_BOLTEK_RAW, mysql);
            strikes.push_back(strike);
            count++;
        } catch(StrikeData::Exception&) {}
        
    } while (mysql.nextRow());
    return count;
}

size_t StrikesGroup::readWithCustomQuery(TableType tableType, MySQLConnectionManager& database, const std::string& query)
{
    /// @todo Add more global functionality to resolve names of table and other information
    std::string table;
    switch(tableType)
    {
        case TT_BOLTEK_RAW: table = "strikes"; break;
        case TT_UNIFIED: table = "unified_strikes"; break;
        /// @todo Add exception for this case
        default: throw Exception();
    }
    
    //std::cout << "QUERYING..." << std::endl;
    if (not database.query("SELECT * FROM " + table + " WHERE " + query + ";"))
        return 0;
    //std::cout << "DONE" << std::endl;
    
    timeClusterStatus = CS_UNDEFINED;
    //std::cout << "Loading strikes..." << std::endl;
    int count = loadStrikes(tableType, database);
    //std::cout << "Loading strikes done" << std::endl;
    sortStrikes();
    return count;
}

size_t StrikesGroup::smartReadStrikes(TableType tableType, MySQLConnectionManager& database, const Time& from, const Time& to)
{
    std::ostringstream baseQuery;
    baseQuery << "SELECT * FROM strikes WHERE "
        << generateTimeConditions(tableType, from, to) << ";";
    
    if (not database.query(baseQuery.str()))
        return 0;
        
    timeClusterStatus = CS_UNDEFINED;
    
    size_t count = loadStrikes(tableType, database);
    
    count += smartReadTail(tableType, database, false);
    count += smartReadTail(tableType, database, true);
    
    sortStrikes();
    return count;
}

size_t StrikesGroup::smartReadTail(TableType tableType, MySQLConnectionManager& database, bool readFromBegin)
{
    size_t result = smartReadTailWithoutSort(tableType, database, readFromBegin);
    if (result != 0)
        sortStrikes();
    return result;
}

size_t StrikesGroup::smartReadTailWithoutSort(TableType tableType, MySQLConnectionManager& database, bool readFromBegin)
{
    size_t count = 0, countTotal = 0;
    if (strikes.empty())
        return 0;
    do {
        std::cout << "Reading tail" << std::endl;
        std::ostringstream baseQuery;
        if (readFromBegin)
        {
            baseQuery << "SELECT * FROM strikes WHERE "
            << generateTimeConditions(
                    tableType, strikeTimeMinMax.getMin() - Configuration::Instance().boltek.getMaxTimeShiftInCluster(),
                    strikeTimeMinMax.getMin(),
                    strikes[strikeTimeMinMax.getMinAttached()].getServerDatabaseId()
                    );
        }
        else
        {
            baseQuery << "SELECT * FROM strikes WHERE "
                << generateTimeConditions(
                    tableType, strikeTimeMinMax.getMax(),
                    strikeTimeMinMax.getMax() + Configuration::Instance().boltek.getMaxTimeShiftInCluster(),
                    strikes[strikeTimeMinMax.getMaxAttached()].getServerDatabaseId()
                    );
        }
        bool queryResult = database.query(baseQuery.str());
        if (queryResult)
        {
            count = loadStrikes(tableType, database);
            countTotal += count;
        } else
            count = 0;
        
    } while (count != 0);
    return countTotal;
}

bool StrikesGroup::testIfInOneCluster(size_t first, size_t second)
{
    if ( (fabs(strikes[second].time - strikes[first].time
              + getTimeShift(strikes[first], strikes[second], Configuration::Instance().precision.timeClusterDetectionAllowedTimeError * 0.5)
              ) - Configuration::Instance().precision.timeClusterDetectionAllowedTimeError
         ) * Global::LightSpeed
         < (strikes[first].position - strikes[second].position)
       )
        return true;
    else
        return false;
}

int StrikesGroup::detectTimeClusters()
{
    /// @todo [low] Some list::size calls can be optimized when it is simply checking if size == 1
    if (timeClusterStatus != CS_UNDEFINED)
        return 0;
    
    if (strikes.size() == 0) {
        // If it is not already set for some reason
        timeClusterStatus = CS_EMPTY;
        return 0;
    }
    
    timeClusters.clear();
    isStrikeInGoodCluster.clear();
    
    // Creating array with 'dirty flags'
    std::vector<bool> inCluster;
    inCluster.reserve(strikes.size());
    for (size_t i=0; i<strikes.size(); i++)
        inCluster[i] = false;
    
    const double maxTimeShift = 2 * Configuration::Instance().boltek.maxDetectionDistance / Global::LightSpeed;
    
    // Moving out from cycle to prevent new/delete calls
    TimeCluster newCluster;
    
    // Main cycle for clusters finding
    for (size_t i=0; i<strikes.size(); i++)
    {
		newCluster.clear();
        // We suggest that cluster begins with strikes[i]        
        newCluster.indexes.push_back(i);
        for (size_t j=i+1;
             j<strikes.size() && (strikes[j].time - strikes[i].time) <= maxTimeShift;
             j++)
        {
            // So strikes[j] COULD be in one cluster with strikes[i]. Lets test it with other elements
            bool canBeAdded = true;
            for (auto it = newCluster.indexes.begin(); it != newCluster.indexes.end(); it++)
            {
                if (not testIfInOneCluster(*it, j))
                    { canBeAdded=false; break; }
            }
            if (canBeAdded) {
                // So this strike can be added to current cluster
                newCluster.indexes.push_back(j);
            }
        }
        // No we have cluster-candidate. We should check if it is really new cluster
        // or all it's participants were added before in bigger cluster.
        
        bool weHaveNewCluster=false; // Flag if we have really new cluster
        auto it = newCluster.indexes.begin();
        
        for (; it != newCluster.indexes.end(); it++)
        {
            if (not inCluster[*it])
                { weHaveNewCluster=true; break; }
        }
        
        if (weHaveNewCluster) {
            // Here we know that we have found really new cluster.
            // Let's add it
            timeClusters.push_back(newCluster);
            // And set dirty flags:
            for (; it != newCluster.indexes.end(); it++)
                inCluster[*it] = true;
        }
    }
    
    // Now we should find mark that are not one in cluster
    for (size_t i = 0; i != strikes.size(); i++)
        isStrikeInGoodCluster.push_back(false);
    
    for (auto it : timeClusters)
    {
        if (it.indexes.size() > 1)
            for (auto ind : it.indexes)
                isStrikeInGoodCluster[ind] = true;
    }
    
    if (timeClusters.size() == 1)
        timeClusterStatus = CS_ONE_CLUSTER;
    else
        timeClusterStatus = CS_MANY_CLUSTERS;
    
    return timeClusters.size();
}

void StrikesGroup::putCluster(size_t index, StrikesGroup& whereToPut)
{
    if (timeClusterStatus != CS_MANY_CLUSTERS)
        throw ExInvalidTimeClusterStatus();
        
    TimeClusterStatus oldTargetTCStastus = whereToPut.timeClusterStatus;
    
    TimeCluster newTimeCluster;
    /// @todo add exception if index > count of clusters
    for (auto it=timeClusters[index].indexes.begin(); it!=timeClusters[index].indexes.end(); it++)
    {
        newTimeCluster.indexes.push_back( whereToPut.insertStrike(strikes[*it]) ); 
    }
    
    switch(oldTargetTCStastus)
    {
        case CS_MANY_CLUSTERS:
        case CS_ONE_CLUSTER:    // So now it is automatically CS_UNDEFINED
        case CS_UNDEFINED: break;
        case CS_EMPTY:
            whereToPut.timeClusterStatus = CS_ONE_CLUSTER;
            whereToPut.timeClusters.push_back(newTimeCluster);
    }
    // whereToPut.findAllMinMax(); - no need, it is automatically counted when insertStrike is called
}

void StrikesGroup::sortStrikes()
{
    std::sort(strikes.begin(), strikes.end());
    findAllMinMax();
}

std::string StrikesGroup::generateTimeConditions(TableType tableType, const Time& from, const Time& to, size_t id)
{
    std::ostringstream oss;
    oss << std::setprecision (std::numeric_limits<double>::digits10 + 1);
    if (id != 0)
        oss << "(";
    switch(tableType)
    {
        case TT_UNIFIED:
            oss << "(round_time>'" << from.getRoundPart() << "' "
                << "|| (round_time='" << from.getRoundPart() << "' "
                << "&& fraction_time>" << from.getFractionPart() << ")) "
                << "&&"
                << "(round_time<'" << to.getRoundPart() << "' "
                << "|| (round_time='" << to.getRoundPart() << "' "
                << "&& fraction_time<" << to.getFractionPart() << "))";
            break;
        case TT_BOLTEK_RAW:
            oss << "(`when`>'" << from.getRoundPart() << "' "
                << "|| (`when`='" << from.getRoundPart() << "' "
                << "&& count_osc>" << from.getFractionPart()*50e6 << ")) "
                << "&&"
                << "(`when`<'" << to.getRoundPart() << "' "
                << "|| (`when`='" << to.getRoundPart() << "' "
                << "&& count_osc<" << to.getFractionPart()*50e6 << "))";
            break;
        default:
            throw std::logic_error(EX_PREFIX + " Cannot generate time conditions for unknown table type");
    }
    if (id != 0)
        oss << ") && `global_id` != " << id;
    return oss.str();
}

void StrikesGroup::writeTimeClusters(MySQLConnectionManager& mysql)
{/*
    if (timeClusterStatus == CS_UNDEFINED)
        throw ExInvalidTimeClusterStatus();*/
    for(auto it = timeClusters.begin(); it != timeClusters.end(); it++)
    {
        // Skip time clusters with 1 strike
        if (it->indexes.size() == 1)
            continue;
        Time clusterBeginTime = strikes[it->indexes.front()].time;
        Time clusterEndTime = strikes[it->indexes.back()].time;
        double clusterDuration = clusterEndTime - clusterBeginTime;
        Time clusterMiddleTime = clusterBeginTime + clusterDuration / 2.0;
        
        std::string strikesList;
        for (auto jt = it->indexes.begin(); jt != it->indexes.end(); jt++)
        {
            strikesList += std::to_string(strikes[*jt].getServerDatabaseId()) + ", ";
        }
        
        MySQLInsertGenerator insert("time_clusters");
        insert.set("round_time",    clusterMiddleTime.getRoundPart(), true);
        insert.set("fraction_time", clusterMiddleTime.getFractionPart());
        insert.set("strikes", strikesList, true);
        insert.set("strikes_count", it->indexes.size());
        mysql << insert;
    }
    
    if (timeClusterStatus == CS_ONE_CLUSTER)
    {
        timeClusterId = mysql.last_insert_id();
    }
}

unsigned int StrikesGroup::readTimeCluster(MySQLConnectionManager& mysql)
{
    unsigned int count = 0;
    try
    {
        std::string clusterText = mysql.getData(Unified::TC_STRIKES);
        std::vector< std::string > strikeIdsList;
        boost::split(strikeIdsList, clusterText, boost::is_any_of(","));
        for (auto it = strikeIdsList.begin(); it != strikeIdsList.end(); it++)
        {
            boost::trim(*it);
            if (*it == "") continue;
            StrikeData strike;
            strike.read(TT_UNIFIED, mysql, std::stoi(*it));
            insertStrike(strike);
            count++;
        }
    }
    catch(StrikeData::Exception&)
    {
        throw ExCannotAddStrikeFromDb();
    }
    catch(const std::exception &ex)
    {
        throw ExInvalidTimeClusterFormat();
    }
    timeClusterStatus = CS_ONE_CLUSTER;
    return count;
}

unsigned int StrikesGroup::readTimeCluster(MySQLConnectionManager& mysql, int id)
{
    std::string query = "SELECT * FROM time_clusters WHERE id='" + std::to_string(id) + "'";
    if (not mysql.query(query))
        throw ExNoSuchTimeCluster();
    
    size_t count = readTimeCluster(mysql);
    timeClusterId = id;
    return count;
}

StrikesGroup::TimeClusterStatus StrikesGroup::getTimeClusterStatus()
{
    return timeClusterStatus;
}

/*
void StrikesGroup::plotDetectionFunction(double latFrom, double latTo,
                                  double lonFrom, double lonTo,
                                  unsigned int latPoints, unsigned int lonPoints,
                                  const std::string& filenamePrefix)
{
    std::ofstream detFuncGraph(filenamePrefix + "-detection-func-graph.txt");
    doPreSolvingJobs();
    for (unsigned int i=0; i<lonPoints; i++)
    {
        for (unsigned int j=0; j<latPoints; j++)
        {
            double lat = latFrom + (latTo-latFrom) * j / latPoints;
            double lon = lonFrom + (lonTo-lonFrom) * i / lonPoints;
            detFuncGraph << lon << " " << lat << " " << calculateDetectionFunction(lat, lon) << std::endl;
        }
        detFuncGraph << std::endl;
    }
    detFuncGraph.close();
    
    std::ofstream dfPositions(filenamePrefix + "-DF-positions.txt");
    for (auto it = strikes.begin(); it != strikes.end(); it++)
    {
        dfPositions << it->position.getLongitude() << " " << it->position.getLatitude()
            << " " << 0 << std::endl;
    }
    dfPositions.close();
}

void StrikesGroup::plotDetectionFunction(double margins,
                                        unsigned int latPoints, unsigned int lonPoints,
                                        const std::string& filenamePrefix)
{
    plotDetectionFunction(latitudeMinMax.getMin() - margins, latitudeMinMax.getMax() + margins,
                          longitudeMinMax.getMin() - margins, longitudeMinMax.getMax() + margins,
                          latPoints, lonPoints,
                          filenamePrefix);
}
*/

void StrikesGroup::writeAllStrikesToDatabase(MySQLConnectionManager& mysql, bool onlyClustersWithMoreThenOneStrike)
{
    if (onlyClustersWithMoreThenOneStrike && timeClusterStatus == CS_UNDEFINED)
        throw std::logic_error(EX_PREFIX +
            "Invalid time cluster status: time clusters not determined but writeAllStrikesToDatabase called with onlyClustersWithMoreThenOneStrike == true");
    
    for(size_t i = 0; i != strikes.size(); i++)
    {
        if (!onlyClustersWithMoreThenOneStrike || isStrikeInGoodCluster[i])
            strikes[i].writeUnifiedData(mysql);
    }
}

bool StrikesGroup::isEnoughData()
{
    if (strikes.size() == 0)
        return false;
    if (strikes.size() < 3 && !Configuration::Instance().solving.useDirectionFinding)
        return false;
    return true;
}

void StrikesGroup::writeSolutionsToFile(const std::string& filePrefix)
{/*
    std::ofstream file(filePrefix+"-solutions.txt");
    for (auto it = lightnings.begin(); it != lightnings.end(); it++)
    {
        file << it->position.getLongitude() << " " << it->position.getLatitude() << " " << calculateDetectionFunction(it->position) << std::endl;
    }
    file.close();*/
}

void StrikesGroup::acceptModifier(IStrikeDataModifier* modifier)
{
    for (auto it = strikes.begin(); it != strikes.end(); it++)
        it->acceptModifier(modifier);
}

Time StrikesGroup::getLatestStrikeTime()
{
    return strikeTimeMinMax.getMax();
}
