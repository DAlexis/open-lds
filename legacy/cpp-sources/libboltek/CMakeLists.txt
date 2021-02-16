cmake_minimum_required(VERSION 3.1)

project(boltek)

#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC -Wall")
#set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -O0 -g -D_DEBUG")
#set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O3 -g -DNDEBUG")

set(LIB_SOURCE
    libboltek.c
)

add_library(${PROJECT_NAME} STATIC ${LIB_SOURCE})

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR})
