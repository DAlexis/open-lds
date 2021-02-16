cmake_minimum_required(VERSION 3.1)

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

set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 11)

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR} ${PROJECT_SOURCE_DIR}/..)
target_link_libraries(${PROJECT_NAME} PUBLIC system-utils blconfig Boost::filesystem)
