cmake_minimum_required(VERSION 3.1.0 FATAL_ERROR)

project(LightningDetectionNetwork)

# Adding custom cmake modules i.e. for MySQL
set(CMAKE_MODULE_PATH "${LightningDetectionNetwork_SOURCE_DIR}/cmake;/usr/share/apps/cmake/modules;${CMAKE_MODULE_PATH};")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
#set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -g -pg -DDEBUG")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -g -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -g -DNDEBUG")

execute_process(COMMAND git log -1 --format="%H" OUTPUT_VARIABLE git_commit)
add_definitions( -DGIT_COMMIT_TAG=${git_commit} )

#add_definitions( -DBOOST_LOG_DYN_LINK )

include(detect-gtest.cmake)

add_subdirectory(libboltek)
add_subdirectory(utils)
add_subdirectory(libdfmath)
add_subdirectory(dfclient)
add_subdirectory(mathtest)
add_subdirectory(strikesprocd)
add_subdirectory(strikesstatd)
#add_subdirectory(strikesproc)

if(GTEST_FOUND)
    # To enable ctest usage
    enable_testing()
    add_subdirectory(unit-tests)
endif()
