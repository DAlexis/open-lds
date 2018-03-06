cmake_minimum_required(VERSION 2.8)

project(timestamp-utils)

find_package (Boost COMPONENTS date_time REQUIRED)


set(LIB_SOURCE
    timestamp-utils.cpp
)

include_directories(${libboltek_INCLUDE_DIRS})

add_library(${PROJECT_NAME} STATIC ${LIB_SOURCE})

export_include_dirs(
    ${PROJECT_SOURCE_DIR}
    ${libboltek_INCLUDE_DIRS}
)

export_libs(
	${PROJECT_NAME}
	${Boost_DATE_TIME_LIBRARY}
)