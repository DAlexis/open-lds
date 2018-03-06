cmake_minimum_required(VERSION 2.8)

project(dfclient)

find_package (Boost COMPONENTS system filesystem date_time REQUIRED)
find_package (Threads)

set(EXE_SOURCES 
    src/main.cpp
    src/receiver-controller.cpp
    src/dfclient-system.cpp
    src/data-collector.cpp
    src/strike-data-container-boltek.cpp
    src/output-provider-interface.cpp
    src/output-provider-mysql.cpp
    src/output-provider-serializer.cpp
    src/health-monitor.cpp
    src/hardware-interface.cpp
    src/boltek-hw-simulator.cpp
    src/boltek-hw.cpp
    src/utils.cpp
)

include_directories(
	${dfclient_SOURCE_DIR}
	${dfclient_SOURCE_DIR}/include
	
    ${timestamp-utils_INCLUDE_DIRS}
    ${positioning-utils_INCLUDE_DIRS}
    ${libboltek_INCLUDE_DIRS}
    ${mysql-wrapper_INCLUDE_DIRS}
    ${serialization-utils_INCLUDE_DIRS}
    ${system-utils_INCLUDE_DIRS}
    ${utils_INCLUDE_DIRS}
    ${configuration-utils_INCLUDE_DIRS}
    ${blconfig_INCLUDE_DIRS}
    ${light-serializer_INCLUDE_DIRS}
)

add_executable(${PROJECT_NAME} ${EXE_SOURCES})

target_link_libraries(${PROJECT_NAME}
    ${timestamp-utils_LIBRARY}
    ${mysql-wrapper_LIBRARY}
    ${system-utils_LIBRARY}
    ${libboltek_LIBRARY}
    ${configuration-utils_LIBRARY}
    ${blconfig_LIBRARY}
    ${positioning-utils_LIBRARY}
    ${light-serializer_LIBRARY}
    
    ${Boost_FILESYSTEM_LIBRARY}
    ${Boost_SYSTEM_LIBRARY}
    ${CMAKE_THREAD_LIBS_INIT}
)

configure_file(dfclient.conf dfclient.conf COPYONLY)
configure_file(experiment_continue.sh experiment_continue.sh COPYONLY)
configure_file(experiment_start.sh experiment_start.sh COPYONLY)
configure_file(push_data.sh push_data.sh COPYONLY)
configure_file(rsync_dfclient_dir.sh rsync_dfclient_dir.sh COPYONLY)

target_compile_features(${PROJECT_NAME} PUBLIC cxx_rvalue_references)