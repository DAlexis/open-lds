cmake_minimum_required(VERSION 2.8)

project(constants)

set(LIB_SOURCE constants.cpp)

include_directories(
    ${utils_INCLUDE_DIRS}
)

add_library(${PROJECT_NAME} STATIC ${LIB_SOURCE})

target_compile_features(${PROJECT_NAME} PUBLIC cxx_rvalue_references)
target_compile_features(${PROJECT_NAME} PUBLIC cxx_lambdas)

export_include_dirs(
    ${PROJECT_SOURCE_DIR}
    ${utils_INCLUDE_DIRS}
)

export_libs(
	${PROJECT_NAME}
)