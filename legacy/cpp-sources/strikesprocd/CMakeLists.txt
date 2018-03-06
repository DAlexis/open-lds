cmake_minimum_required(VERSION 2.8)

project(strikes-processing-deamon)

include(${LightningDetectionNetwork_SOURCE_DIR}/cmake/EnablePthreads.cmake)

find_package (Boost COMPONENTS filesystem system thread program_options REQUIRED)


set(EXE_SOURCES
    strikes-processing-deamon.cpp
    strikes-processor.cpp
)

set(Boost_USE_STATIC_LIBS        OFF)
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME     OFF)
set(Boost_ALL_DYN_LINK           ON)

add_definitions( -DBOOST_LOG_DYN_LINK )

include_directories(
    ${df-math_INCLUDE_DIRS}
    ${log-config_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIRS}
)

add_executable(${PROJECT_NAME} ${EXE_SOURCES})

target_link_libraries (${PROJECT_NAME}
    ${libboltek_LIBRARY}
    ${df-math_LIBRARY}
    ${blconfig_LIBRARY}
    ${math-utils_LIBRARY}
    ${constants_LIBRARY}
    ${MYSQL_LIBRARY}
    ${Boost_LIBRARIES}
)

configure_file(strike-proc-d.conf strike-proc-d.conf COPYONLY)
configure_file(../libdfmath/df-math.conf df-math.conf COPYONLY)
