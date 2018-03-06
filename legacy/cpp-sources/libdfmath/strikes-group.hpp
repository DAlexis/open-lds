/*
 * Copyright (c) 2013 Institute of Applied Physics of the Russian Academy of Sciences
 */

#ifndef STRIKES_GROUP_H_INCLUDED
#define STRIKES_GROUP_H_INCLUDED

#include "strike-data.hpp"
#include "lightning-positioning-result.hpp"
#include "math-utils.hpp"

#include <vector>
#include <list>

/// This class operates with variety of strikes. Clasterisation and finding position is it's job.
class StrikesGroup
{
public:
    enum TimeClusterStatus
    {
        CS_UNDEFINED = 0,
        CS_EMPTY,
        CS_ONE_CLUSTER,
        CS_MANY_CLUSTERS
    };
    
    StrikesGroup();
    ~StrikesGroup();
    
    /// Simply add strike to this group
    size_t insertStrike(StrikeData& strike);
    
    /// Remove all data from group
    void clear();
    
    /** @brief Read strikes from boltek's database with ids in [firstId, lastId]
     */    
    size_t readBoltekStrikesWithIdsInRange(MySQLConnectionManager& boltek, size_t firstId, size_t lastId);
    
    /// Read from any database with custom query. "SELECT ... FROM ... WHERE" will be added automatically so you need only condition
    size_t readWithCustomQuery(TableType tableType, MySQLConnectionManager& database, const std::string& query);
    
    /** @brief Read strikes that could be in one cluster with those in group
     * @param tableType     Type of table to generate query
     * @param database      Database connection
     * @param readFromBegin Reading 'tail' near strikes group beginning (by time axis) means true, reading near end means false (default)
     * @return Count of readed records
     */    
    size_t smartReadTail(TableType tableType, MySQLConnectionManager& database, bool readFromBegin = false);
    
    /** @brief Read strikes from database. Garatied that strikes closer then distance/LIGHT_SPEED to anyone added will be added too
     * @param from     Time since strikes would be added
     * @param to       Time before which strikes would be added
     * @return Count of readed records
     */    
    size_t smartReadStrikes(TableType tableType, MySQLConnectionManager& database, const Time& from, const Time& to);
    
    Time getLatestStrikeTime(); /// @todo Add const here
    
    /** @brief Detect time groups of strikes when neighbors are closer then distance/LIGHT_SPEED
     * @param distance May be maximal distance between DF stations
     */ 
    int detectTimeClusters();
    void writeTimeClusters(MySQLConnectionManager& mysql);
    //void separateLastCluster(StrikesGroup& whereToSeparate);
    
    /** @brief Read all strikes from one time cluster
     * @return Count of readed strikes
     */
    unsigned int readTimeCluster(MySQLConnectionManager& mysql);
    /** @brief    Read all strikes from one time cluster
     * @param id  Cluster id in db
     * @return    Count of readed strikes
     */
    unsigned int readTimeCluster(MySQLConnectionManager& mysql, int id);
    
    /// Return current status of time cluster's detection
    TimeClusterStatus getTimeClusterStatus();
    /*
    /// Prepare data for gnuplot for plotting calculateDetectionFunction
    void plotDetectionFunction(double latFrom, double latTo,
                               double lonFrom, double lonTo,
                               unsigned int latPoints, unsigned int lonPoints,
                               const std::string& filenamePrefix);
    
    /// Plot with auto-selection if limits for plotting
    void plotDetectionFunction(double margins,
                               unsigned int latPoints, unsigned int lonPoints,
                               const std::string& filenamePrefix);
    */
    
    /// Return true if detection problem could be solved. For example, if detection finding is disabled and there are only two stations that detected signal
    bool isEnoughData();
    
    /// Write all contained strikes to uniform database
    void writeAllStrikesToDatabase(MySQLConnectionManager& mysql, bool onlyClustersWithMoreThenOneStrike = true);
    
    /// Simply write solutions' positions for gnuplotting
    void writeSolutionsToFile(const std::string& filePrefix);
    
    /** @brief Remove all time cluster that contains lesser or equal maxStrikesCount strikes
     * @return count of dropped clusters
     */
    size_t dropTimeClusters(int maxStrikesCount);
    
    // Accept modifier to all strikes in container
    void acceptModifier(IStrikeDataModifier* modifier);
    
    // Accept many modifiers to all strikes in container
    template<class Iterator>
    void acceptModifiers(Iterator begin, Iterator end)
    {
        for (Iterator it = begin; it != end; it++)
            acceptModifier(*it);
    }
    
    inline std::vector<StrikeData>::iterator begin() { return strikes.begin(); }
    inline std::vector<StrikeData>::iterator end() { return strikes.end(); }
    
    inline StrikeData& front() { return strikes.front(); }
    inline StrikeData& back() { return strikes.back(); }
    
    void putCluster(size_t index, StrikesGroup& whereToPut);
    
    inline StrikeData operator[](size_t index)
    {
        return strikes[index];
    }
    
    size_t size()
    {
        return strikes.size();
    }
    
    inline double getMinLat() { return latitudeMinMax.getMin(); }
    inline double getMaxLat() { return latitudeMinMax.getMax(); }
    inline double getMinLon() { return longitudeMinMax.getMin(); }
    inline double getMaxLon() { return longitudeMinMax.getMax(); }
    
    size_t getTimeClusterId() { return  timeClusterId; }
    
    const std::vector<StrikeData>& getStrikes() { return strikes; }
    
    struct TimeCluster
    {
        void clear() { indexes.clear(); }
        std::list<size_t> indexes;
    };
    
    std::vector< TimeCluster > timeClusters;
    std::vector<StrikeData> strikes;
    
    ///////////////////////
    // Exceptions
    class Exception : public std::exception {};
    
    class ExNoSuchStrike : public Exception
    {
    public:
        virtual const char* what() const throw() { return "Can not find any strike in database"; }
    };
    
    class ExNoSuchTimeCluster : public Exception
    {
    public:
        virtual const char* what() const throw() { return "Can not find such time cluster in database"; }
    };
    
    class ExInvalidTimeClusterStatus : public Exception
    {
    public:
        virtual const char* what() const throw() { return "Invalid time cluster status"; }
    };
    
    class ExInvalidTimeClusterFormat : public Exception
    {
    public:
        virtual const char* what() const throw() { return "Time cluster format in db is invalid"; }
    };
    
    class ExCannotAddStrikeFromDb : public Exception
    {
    public:
        virtual const char* what() const throw() { return "Cannot add strike specified in cluster. May be it does not exists"; }
    };
    
    class ExMoreThenOneCluster : public Exception
    {
    public:
        virtual const char* what() const throw() { return "Strike group may contain more then one time cluster or empty, can not run direction finding algorythm"; }
    };
    
private:
    
    bool testIfInOneCluster(size_t first, size_t second);
    
    /** @brief Load all returned rows from mysql from table with specified type
     * @return Count of loaded strikess
     */
    int loadStrikes(TableType tableType, MySQLConnectionManager& mysql);
    
    int loadStrikesUnified(MySQLConnectionManager& mysql);
    
    /** @brief Load all returned rows from mysql for boltek table
     * @return Count of loaded strikess
     */
    int loadStrikesBoltek(MySQLConnectionManager& mysql);
    
    /// Internal function: does all job but does not sort
    size_t smartReadTailWithoutSort(TableType tableType, MySQLConnectionManager& database, bool readFromBegin = false);
    
    /** @brief Generate conditions for mysql
    * @param id            Not zero if condition `id` != id should be added
    */
    std::string generateTimeConditions(TableType tableType, const Time& from, const Time& to, size_t id = 0);
    
    void sortStrikes();
    
    
    std::vector<bool>       isStrikeInGoodCluster; // true if this strike is in cluster with more than 1 strikes (has "neighbours")
    
    MinMaxFinderWithData<Time, size_t> strikeTimeMinMax; ///< Second template argument means index in strikes array
    MinMaxFinderWithData<double, size_t> latitudeMinMax;
    MinMaxFinderWithData<double, size_t> longitudeMinMax;
    
    void testMinMax(size_t index);
    void minMaxTestRange(size_t from, size_t to);
    void findAllMinMax();
    
    TimeClusterStatus timeClusterStatus;
    
    /// If this is one time cluster timeClusterId stores it's id when avaliable
    size_t timeClusterId;
};

#endif //STRIKES_GROUP_H_INCLUDED
