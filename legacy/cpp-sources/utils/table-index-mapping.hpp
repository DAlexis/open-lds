/*
 * Copyright (c) 2013 Institute of Applied Physics of the Russian Academy of Sciences
 */

#ifndef TABLE_INDEX_MAPPING_H_INCLUDED
#define TABLE_INDEX_MAPPING_H_INCLUDED

enum TableType
{
    TT_UNIFIED = 0,
    TT_BOLTEK_RAW,
    TT_COUNT,
};

enum SolutionStatus
{
    SS_GOOD = 0,
    SS_ON_THE_FAREST_BORDED = 1,
};

class Boltek
{
public:
    /// Column index in MySQL table with boltek raw data. 'RD' means 'Raw Data'
    enum BoltekRawDataIndex
    {
        RD_ID = 0, /* Fedor added field global_id to base implying that botek_raw_data db is installed on every df station,
                    * so every station would have it's own strike's id's.
                    * 
                    * So, "RD_ID" means mysql field "global_id".
                    * To skip then mysql field "id" I've added to enum "RD_OLD_ID"
                    */
        RD_OLD_ID,
        RD_DEVICE_ID,
        RD_EXPERIMENT_ID,
        RD_WHEN,
        RD_OSC_FREQ,
        RD_COUNT_OSC,
        RD_WHEN_DATE, // Added by Fedor (to simplify searching?)
        RD_LON,
        RD_LAT,
        RD_LON_EW,
        RD_LAT_NS,
        RD_E_FIELD,
        RD_MN_FIELD,
        RD_ME_FIELD,
        RD_COUNT, /// Contains count of columns
    };
};

class Unified
{
public:
    /// Column index in MySQL table with unified strike data. 'SD' means 'Strike Data'
    enum UnifiedStrikeDataIndex
    {
        SD_ID = 0,
        SD_DEVICE_ID,
        SD_EXPERIMENT_ID,
        SD_ROUNT_TIME,
        SD_FRACTION_TIME,
        SD_BUFFER_DURATION,
        SD_BUFFER_VALUES_COUNT,
        SD_BUFFER_BEGIN_TIME_SHIFT,
        SD_LONGTITUDE,
        SD_LATITUDE,
        SD_LON_CHAR,
        SD_LAT_CHAR,
        SD_E_FIELD,
        SD_MN_FIELD,
        SD_ME_FIELD,
        SD_DIRECTION,
        SD_DIRECTION_ERROR,
        SD_COUNT, /// Contains count of columns
    };
    
    enum TimeClustersDataIndex
    {
        TC_ID = 0,
        TC_ROUNT_TIME,
        TC_FRACTION_TIME,
        TC_STRIKES_COUNT,
        TC_STRIKES,
        TC_COUNT, /// Contains count of columns
    };
};

#endif //TABLE_INDEX_MAPPING_H_INCLUDED
