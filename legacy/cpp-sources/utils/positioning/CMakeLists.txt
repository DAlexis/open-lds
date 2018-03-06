cmake_minimum_required(VERSION 2.8)

project(positioning-utils)

set(LIB_SOURCE positioning.cpp)

set(${PROJECT_NAME}_USED_INCDIRS
    ${timestamp-utils_INCLUDE_DIRS}
    ${constants_INCLUDE_DIRS}
    ${math-utils_INCLUDE_DIRS}
)

include_directories(
    ${${PROJECT_NAME}_USED_INCDIRS}
)

add_library(${PROJECT_NAME} STATIC ${LIB_SOURCE})

target_compile_features(${PROJECT_NAME} PUBLIC cxx_rvalue_references)
target_compile_features(${PROJECT_NAME} PUBLIC cxx_lambdas)

export_include_dirs(
    ${PROJECT_SOURCE_DIR}
    ${${PROJECT_NAME}_USED_INCDIRS}
)

export_libs(
	${PROJECT_NAME}
	${timestamp-utils_LIBRARY}
	${math-utils_LIBRARY}
	${constants_LIBRARY}
)