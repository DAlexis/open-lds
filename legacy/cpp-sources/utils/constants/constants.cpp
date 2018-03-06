#include "constants.hpp"

#include <stddef.h>
#include <sstream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

Configuration* Configuration::instance = NULL;

Configuration::ConfigurationInitializer Configuration::initializer;

Configuration::ConfigurationInitializer::ConfigurationInitializer()
{
    Configuration::instance = new Configuration;
}

Configuration::ConfigurationInitializer::~ConfigurationInitializer()
{
    if (Configuration::instance) delete Configuration::instance;
}

Configuration::Configuration()
{
    // Setting up default parameters
    boltek.maxDetectionDistance = 400000;
    
    /// @todo here!
    //boltek.bufferDuration = 64e-6;
    
    
    // Setting precision parameters
    precision.timePrecision = 1e-7;
    precision.timeClusterDetectionAllowedTimeError = 8e-6;
    
    // Lightning positioning problem solving
    solving.useTimeOfArrival = true;
    solving.useDirectionFinding = true;
    
    solving.clusterRadius = 1500;
    solving.GDSInitialStep = 1e-2;
    solving.GDSPrecision = 2e-4;
    solving.latLonMargins = 1.0;
    solving.initPointsGridLinesCount = 10;
    // Weights
    solving.timeOfArrivalWeight = 200.0;
    solving.directionFindingWeight = 1.0;
    solving.finiteRadiusTermWeight = 4.0;
    
    solving.finiteRadiusTermWeightSmoothingRadius = 30000;
    
    solving.threadsCount = 1;
}


double Configuration::BoltekConfig::getMaxTimeShiftInCluster()  const
{
    /// @todo Add here corellation shift error, use it function everywhere
    return maxDetectionDistance / Global::LightSpeed;
}

template<typename T>
inline void setIfExist(const boost::property_tree::ptree& pt, T& whereToPut, const std::string& name)
{
    whereToPut = pt.get<T>(name, whereToPut);
}

void Configuration::readConfigFromFile(const std::string& filename)
{
    boost::property_tree::ptree pt;
    try {
        boost::property_tree::ini_parser::read_ini(filename, pt);
        setIfExist(pt, Configuration::instance->boltek.maxDetectionDistance,    "boltek.max_detection_distance");
        
        setIfExist(pt, Configuration::instance->precision.timePrecision,        "precision.time_precision");
        setIfExist(pt, Configuration::instance->precision.timeClusterDetectionAllowedTimeError, "precision.time_cluster_detection_allowed_time_error");
        
        setIfExist(pt, Configuration::instance->solving.useTimeOfArrival,       "solving.use_time_of_arrival");
        setIfExist(pt, Configuration::instance->solving.useDirectionFinding,    "solving.use_direction_finding");
        setIfExist(pt, Configuration::instance->solving.clusterRadius,          "solving.cluster_radius");
        setIfExist(pt, Configuration::instance->solving.GDSInitialStep,         "solving.GDS_initial_step");
        setIfExist(pt, Configuration::instance->solving.GDSPrecision,           "solving.GDS_precision");
        setIfExist(pt, Configuration::instance->solving.latLonMargins,          "solving.lat_lon_margins");
        setIfExist(pt, Configuration::instance->solving.initPointsGridLinesCount,   "solving.init_points_grid_lines_count");
        setIfExist(pt, Configuration::instance->solving.timeOfArrivalWeight,   "solving.time_of_arrival_weight");
        setIfExist(pt, Configuration::instance->solving.directionFindingWeight,   "solving.direction_finding_weight");
        setIfExist(pt, Configuration::instance->solving.finiteRadiusTermWeight,   "solving.finite_radius_term_weight");
        setIfExist(pt, Configuration::instance->solving.finiteRadiusTermWeightSmoothingRadius,   "solving.finite_radius_term_weight_smoothing_radius");
        
        setIfExist(pt, Configuration::instance->solving.threadsCount,   "solving.threads_count");
        
        // Checking for logic errors:
        if (!Configuration::instance->solving.useTimeOfArrival
            && !Configuration::instance->solving.useDirectionFinding)
            throw std::logic_error(EX_PREFIX + " Time of arrival and Direction finding methods cannot be disabled at the same time");
    }
     
    catch(boost::property_tree::ini_parser::ini_parser_error &exception)
    {
        //std::ostringstream message
        throw std::logic_error(EX_PREFIX + ".ini file parsing error in " + exception.filename() +  ":" + std::to_string(exception.line()) + " - " + exception.message() );
    }
    catch(boost::property_tree::ptree_error &exception)
    {
        throw std::logic_error(EX_PREFIX + "Parsing error: " + exception.what());
    }
}
