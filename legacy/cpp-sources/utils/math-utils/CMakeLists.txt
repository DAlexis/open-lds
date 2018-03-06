cmake_minimum_required(VERSION 2.8)

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

target_compile_features(${PROJECT_NAME} PUBLIC cxx_rvalue_references)
target_compile_features(${PROJECT_NAME} PUBLIC cxx_lambdas)

export_include_dirs(
    ${PROJECT_SOURCE_DIR}
)

export_libs(
	${PROJECT_NAME}
	${CMAKE_THREAD_LIBS_INIT}
)