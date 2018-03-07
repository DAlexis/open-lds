cmake_minimum_required(VERSION 2.8)

project(utils)

export_include_dirs(
    ${PROJECT_SOURCE_DIR}
)

add_subdirectory(boost-log-configurator)
add_subdirectory(constants)
add_subdirectory(math-utils)
add_subdirectory(timestamp)
add_subdirectory(positioning)
add_subdirectory(mysql)
add_subdirectory(light-serializer)
add_subdirectory(system)
add_subdirectory(configuration)
