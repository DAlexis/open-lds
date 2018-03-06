cmake_minimum_required(VERSION 3.1.0 FATAL_ERROR)

project(LightningDetectionNetwork)

# Adding custom cmake modules i.e. for MySQL
set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};/usr/share/apps/cmake/modules;${LightningDetectionNetwork_SOURCE_DIR}/cmake")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
#set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -g -pg -DDEBUG")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -g -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -g -DNDEBUG")

execute_process(COMMAND git log -1 --format="%H" OUTPUT_VARIABLE git_commit)
add_definitions( -DGIT_COMMIT_TAG=${git_commit} )

add_definitions( -DBOOST_LOG_DYN_LINK )

# This is to run all tests using 'make run-unit-tests' command.
# Another way is simply run 'ctest -VV', but output is not colorized :(
add_custom_target(run-unit-tests)
macro(run_test test_target)
  add_custom_target(${test_target}_runtest
      COMMAND ${test_target} #cmake 2.6 required
      DEPENDS ${test_target}
      WORKING_DIRECTORY "${CMAKE_BINARY_DIR}")
  add_dependencies(run-unit-tests ${test_target}_runtest)
endmacro()

## This macro creates variable ${PROJECT_NAME}_INCLUDE_DIRS with list of 
# include directories exported by project
macro(export_include_dirs)
    set(${PROJECT_NAME}_INCLUDE_DIRS
        ${ARGV}
        CACHE INTERNAL "${PROJECT_NAME}: Include Directories" FORCE)
endmacro()

# This macro creates variable ${PROJECT_NAME}_LIBRARY with list of libraries exported 
# by project
macro(export_libs)
    set(${PROJECT_NAME}_LIBRARY
        ${ARGV}
        CACHE INTERNAL "${PROJECT_NAME}: used and exported libs" FORCE)
endmacro()

# To enable ctest usage
enable_testing()

add_subdirectory(libboltek)
add_subdirectory(utils)
add_subdirectory(libdfmath)
add_subdirectory(dfclient)
add_subdirectory(mathtest)
add_subdirectory(strikesprocd)
add_subdirectory(strikesstatd)
add_subdirectory(strikesproc)

add_subdirectory(unit-tests)