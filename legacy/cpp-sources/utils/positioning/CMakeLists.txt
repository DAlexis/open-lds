cmake_minimum_required(VERSION 3.1)

project(positioning-utils)

set(LIB_SOURCE positioning.cpp)

include_directories(
    ${${PROJECT_NAME}_USED_INCDIRS}
)

add_library(${PROJECT_NAME} STATIC ${LIB_SOURCE})

set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 11)

target_link_libraries(${PROJECT_NAME} PUBLIC timestamp-utils math-utils constants)
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR})
