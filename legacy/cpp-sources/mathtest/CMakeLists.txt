cmake_minimum_required(VERSION 2.8)

project(mathtest)

include(${LightningDetectionNetwork_SOURCE_DIR}/cmake/EnablePthreads.cmake)

find_package (Boost COMPONENTS date_time program_options REQUIRED)

set(EXE_SOURCES mathtest.cpp)

include_directories(
    ${df-math_INCLUDE_DIRS}
)

add_executable(${PROJECT_NAME} ${EXE_SOURCES})

target_link_libraries (${PROJECT_NAME}
    ${df-math_LIBRARY}
    ${MYSQL_LIBRARY}
    ${Boost_LIBRARIES}
    ${blconfig_LIBRARY}
)

configure_file(scripts/clasterisation.sh scripts/clasterisation.sh COPYONLY)
configure_file(scripts/detection-func.plt scripts/detection-func.plt COPYONLY)
configure_file(scripts/find-direction-and-plot-fields.sh scripts/find-direction-and-plot-fields.sh COPYONLY)
configure_file(scripts/generate-random-case scripts/generate-random-case COPYONLY)
configure_file(scripts/move-all-with-custiom-query.sh scripts/move-all-with-custiom-query.sh COPYONLY)  
configure_file(scripts/move-to-unified-base.sh scripts/move-to-unified-base.sh COPYONLY)
configure_file(scripts/plot-detection-func.sh scripts/plot-detection-func.sh COPYONLY)