cmake_minimum_required(VERSION 2.8)

project(strikes-processor)

include(${LightningDetectionNetwork_SOURCE_DIR}/cmake/EnablePthreads.cmake)

find_package (Boost COMPONENTS log log_setup filesystem system thread date_time program_options REQUIRED)


set(EXE_SOURCES
    strikes-proc.cpp
)

set(Boost_USE_STATIC_LIBS        OFF)
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME     OFF)
set(Boost_ALL_DYN_LINK           ON)

add_definitions( -DBOOST_LOG_DYN_LINK )

include_directories(
    ${df-math_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIRS}
)

add_executable(${PROJECT_NAME} ${EXE_SOURCES})

target_link_libraries (${PROJECT_NAME}
    mysql-wrapper
    boltek
    df-math
    math-utils
    timestamp-utils
    positioning-utils
    constants
    ${MYSQL_LIBRARY}
    ${blconfig_LIBRARY}
    ${Boost_LIBRARIES}
)

