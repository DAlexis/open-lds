cmake_minimum_required(VERSION 3.1)

project(math-utils)
find_package (Threads)

set(LIB_SOURCE
    gradient-descent-solver.cpp
    linear-algebra.cpp
    random.cpp
    math-basics.cpp 
    math-utils.cpp
)

add_library(${PROJECT_NAME} STATIC ${LIB_SOURCE})

target_link_libraries(${PROJECT_NAME} PUBLIC ${CMAKE_THREAD_LIBS_INIT})
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR})

set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 11)
