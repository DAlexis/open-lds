cmake_minimum_required(VERSION 2.8)

project(libboltek)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC -Wall")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -O0 -g -D_DEBUG")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O3 -g -DNDEBUG")

set(LIB_SOURCE
    libboltek.c
)

add_library(boltek STATIC ${LIB_SOURCE})

export_include_dirs(
    ${PROJECT_SOURCE_DIR}
)

export_libs(
	boltek
)