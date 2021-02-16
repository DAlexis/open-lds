cmake_minimum_required(VERSION 3.1)

project(timestamp-utils)

find_package (Boost COMPONENTS date_time REQUIRED)


set(LIB_SOURCE
    timestamp-utils.cpp
)

include_directories(${libboltek_INCLUDE_DIRS})

add_library(${PROJECT_NAME} STATIC ${LIB_SOURCE})

set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 11)

target_link_libraries(${PROJECT_NAME} PUBLIC ${Boost_LIBRARIES} boltek)
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR})
