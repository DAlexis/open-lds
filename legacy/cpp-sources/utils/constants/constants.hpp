/*
 * Copyright (c) 2013 Institute of Applied Physics of the Russian Academy of Sciences
 */

#ifndef CONSTANTS_H_INCLUDED
#define CONSTANTS_H_INCLUDED

#include "exceptions.hpp"

#include <string>

class Global
{
public:
    constexpr static double LightSpeed = 299792458;     /// Light speed in meters per sec
    constexpr static double EarthRadius = 6371000;    /// Approximated Earth radius in meters
};

class Configuration
{
private:
    Configuration();
    static Configuration* instance;
    
    class ConfigurationInitializer
    {
    public:
        ConfigurationInitializer();
        ~ConfigurationInitializer();
    };
    static ConfigurationInitializer initializer;
    
public:
    static void readConfigFromFile(const std::string& filename);
    static inline const Configuration& Instance() { return *instance; }
    static inline Configuration& Modify() { return *instance; }
    
    class PrecisionConfig
    {
    public:
        double timePrecision;       /// Times closer then this value  are considered as equal
        double timeClusterDetectionAllowedTimeError;    /// Allowed time error between 2 strikes in one time cluster
    } precision;
    
    class BoltekConfig
    {
    public:
        double maxDetectionDistance; /// Max boltek's detection distance
        double bufferDuration;
        double getMaxTimeShiftInCluster() const;
    } boltek;
    
    class LightningPosSolving
    {
    public:
        bool useTimeOfArrival;      /// Enable time of arrival method
        bool useDirectionFinding;   /// Enable detection finding method
        double clusterRadius;       /// meters - Max cluster radius for solutions clusterization after GDS
        double GDSInitialStep;      /// degrees - First step length for gradient descent solver when minimizing detection function
        double GDSPrecision;        /// degrees - XY-precision for gradient descent solver when minimizing detection function
        double latLonMargins;       /// degrees - Margins for initial points grid for GDS in degreese
        unsigned int initPointsGridLinesCount;  /// Count of cells per one dimension in grid of initial points for GDS
        
        // Weights for terms in detection function
        double timeOfArrivalWeight;     /// Weight of time of arrival method term
        double directionFindingWeight;  /// Weight of direction finding method term
        double finiteRadiusTermWeight;  /// Weight of finite radius terms
        
        double finiteRadiusTermWeightSmoothingRadius;   /// meters - Radius for smoothing border of detection zone
        
        unsigned int threadsCount;
    } solving;
    
    
};

const double TIME_PRECISION = 1e-7;        /// Times closer then this value  are considered as equal
const double NEAREST_ZONE   = 4000;        /// Size of zone near pelengator where should not be lightnings       

#endif //CONSTANTS_H_INCLUDED
