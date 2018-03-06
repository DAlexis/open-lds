cmake_minimum_required(VERSION 2.8)

project(configuration-utils)

find_package (Boost COMPONENTS filesystem REQUIRED)

set(LIB_SOURCE
	configurator.cpp
)

set(USED_INCDIRS
	${utils_INCLUDE_DIRS}
    ${blconfig_INCLUDE_DIRS}
    ${system-utils_INCLUDE_DIRS}
    ${blconfig_INCLUDE_DIRS}
)

include_directories(
    ${USED_INCDIRS}
)

add_library(${PROJECT_NAME} STATIC ${LIB_SOURCE})

target_compile_features(${PROJECT_NAME} PUBLIC cxx_rvalue_references)
target_compile_features(${PROJECT_NAME} PUBLIC cxx_lambdas)

export_include_dirs(
    ${PROJECT_SOURCE_DIR}
    ${USED_INCDIRS}
)

export_libs(
	${PROJECT_NAME}
	${system-utils_LIBRARY}
	${blconfig_LIBRARY}
	${Boost_FILESYSTEM_LIBRARY}
)