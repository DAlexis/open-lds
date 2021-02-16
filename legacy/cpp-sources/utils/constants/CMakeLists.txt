cmake_minimum_required(VERSION 3.1)

project(constants)

set(LIB_SOURCE constants.cpp)

include_directories(
    ${utils_INCLUDE_DIRS}
)

add_library(${PROJECT_NAME} STATIC ${LIB_SOURCE})

set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 11)
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR} ${PROJECT_SOURCE_DIR}/..)
