cmake_minimum_required(VERSION 3.1)

project(df-math)
find_package (Threads)

set(LIB_SOURCE 
    dfmath.cpp
    strike-data.cpp
    strike-modifiers.cpp
    strikes-group.cpp
    strikes-group-solvers.cpp
    strikes-group-solver-variational.cpp
    lightning-positioning-result.cpp
    corellation.cpp
    detection-testing.cpp

    corellation.hpp
    detection-testing.hpp
    dfmath.hpp
    lightning-positioning-result.hpp
    strike-data.hpp
    strike-modifiers.hpp
    strikes-group.hpp
    strikes-group-solvers.hpp
    strikes-group-solver-variational.hpp
)

set(${PROJECT_NAME}_USED_INCDIRS
    ${libboltek_INCLUDE_DIRS}
    ${utils_INCLUDE_DIRS}
    ${mysql-wrapper_INCLUDE_DIRS}
    ${timestamp-utils_INCLUDE_DIRS}
    ${positioning-utils_INCLUDE_DIRS}
    ${math-utils_INCLUDE_DIRS}
    ${constants_INCLUDE_DIRS}
    ${log-config_INCLUDE_DIRS}
)

add_library(${PROJECT_NAME} STATIC ${LIB_SOURCE})

set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 11)
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR} ${boltek_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PUBLIC timestamp-utils mysql-wrapper positioning-utils math-utils constants blconfig ${CMAKE_THREAD_LIBS_INIT})

#export_libs(
#    -Wl,--whole-archive
#    ${PROJECT_NAME}
#    -Wl,--no-whole-archive
    
