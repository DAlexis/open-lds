cmake_minimum_required(VERSION 2.8)

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

include_directories(${${PROJECT_NAME}_USED_INCDIRS})

add_library(${PROJECT_NAME} STATIC ${LIB_SOURCE})

target_compile_features(${PROJECT_NAME} PUBLIC cxx_rvalue_references)
target_compile_features(${PROJECT_NAME} PUBLIC cxx_lambdas)

export_include_dirs(
    ${PROJECT_SOURCE_DIR}
    ${${PROJECT_NAME}_USED_INCDIRS}
)

export_libs(
	-Wl,--whole-archive
	${PROJECT_NAME}
	-Wl,--no-whole-archive
	${timestamp-utils_LIBRARY}
    ${mysql-wrapper_LIBRARY}
    ${positioning-utils_LIBRARY}    
    ${math-utils_LIBRARY}
    ${constants_LIBRARY}
    ${blconfig_LIBRARY}
    ${CMAKE_THREAD_LIBS_INIT}
)
