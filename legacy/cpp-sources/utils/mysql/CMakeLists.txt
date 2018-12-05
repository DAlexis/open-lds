cmake_minimum_required(VERSION 2.8)

project(mysql-wrapper)

find_package(MySQL REQUIRED)

set(LIB_SOURCE
    MysqlWrapper.cpp
)

set(${PROJECT_NAME}_USED_INCDIRS
	${MYSQL_INCLUDE_DIRECTORIES}
	${blconfig_INCLUDE_DIRS}
)

include_directories(
    ${${PROJECT_NAME}_USED_INCDIRS}
)


include_directories()

add_library(${PROJECT_NAME} STATIC ${LIB_SOURCE})

# This enables C++11 compiler key. PUBLIC means enabling it for dependent projects too
target_compile_features(${PROJECT_NAME} PUBLIC cxx_rvalue_references)

export_include_dirs(
    ${PROJECT_SOURCE_DIR}
    ${${PROJECT_NAME}_USED_INCDIRS}
)

export_libs(
	${PROJECT_NAME}
	${MYSQL_LIBRARY}
	${blconfig_LIBRARY}
)
