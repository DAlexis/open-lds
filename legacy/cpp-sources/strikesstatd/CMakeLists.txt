cmake_minimum_required(VERSION 3.1)

project(strikes-statistics-collector)

include(${LightningDetectionNetwork_SOURCE_DIR}/cmake/EnablePthreads.cmake)

find_package (Boost COMPONENTS log log_setup filesystem system thread date_time program_options REQUIRED)


set(EXE_SOURCES strikesstatd.cpp
                configurator.cpp
                stats-calculator.cpp)

#set(Boost_USE_STATIC_LIBS        OFF)
#set(Boost_USE_MULTITHREADED      ON)
#set(Boost_USE_STATIC_RUNTIME     OFF)
#set(Boost_ALL_DYN_LINK           ON)

#add_definitions( -DBOOST_LOG_DYN_LINK )

add_executable(${PROJECT_NAME} ${EXE_SOURCES})

target_link_libraries (${PROJECT_NAME} PUBLIC
    -Wl,--whole-archive
    df-math
    -Wl,--no-whole-archive
    blconfig
    ${Boost_LIBRARIES}
)

configure_file(strikesstatd.conf strikesstatd.conf COPYONLY)
configure_file(../libdfmath/df-math.conf df-math.conf COPYONLY)
