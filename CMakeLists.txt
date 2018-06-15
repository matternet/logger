cmake_minimum_required(VERSION 3.8)

project(src)

set(HOME ${CMAKE_CURRENT_SOURCE_DIR})

find_package(Threads REQUIRED)

set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED OFF)

find_package(Boost COMPONENTS
    program_options # Used for command line arguments
    filesystem # Used to open/close files
REQUIRED)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(Boost_NO_SYSTEM_PATHS true)

add_definitions(-DBOOST_ALL_NO_LIB)
add_definitions(-DBOOST_ALL_DYN_LINK)
add_definitions(-DBOOST_LOG_DYN_LINK)

find_package(CURL REQUIRED)
include_directories(${CURL_INCLUDE_DIRS})

find_library(WIRINGPI NAMES wiringPi)
if(NOT WIRINGPI)
    message(WARNING "WiringPi library not found")
else()
    message(STATUS "WiringPi library found")
    add_definitions(-DWIRINGPI)
    set(SOURCES_TEMPERATURE
        src/temperature-logger.cpp
    )
    set(LIB_WIRINGPI ${WIRINGPI})
endif()

find_library(BITLIB NAMES BitLib)
if(NOT BITLIB)
    message(WARNING "Bitscope library not found")
else()
    message(STATUS "Bitscope library found")
    add_definitions(-DBITLIB)
    set(SOURCES_TEMPERATURE
        src/bitscope-logger.cpp
    )
    set(LIB_BITLIB ${BITLIB})
endif()

include_directories("include/slacking")
# Uncomment this line if you wish to have slacking display
# add_definitions(-DSLACKING_VERBOSE_OUTPUT=1)
include_directories("src")

set (SOURCES_SRC
    src/main.cpp
    src/serial-logger.cpp
    ${SOURCES_TEMPERATURE}
    ${SOURCES_BITSCOPE}
)

add_executable(logger ${SOURCES_SRC})
target_link_libraries(logger
    ${CMAKE_THREAD_LIBS_INIT}
    ${CURL_LIBRARIES}
    ${Boost_LIBRARIES}
    ${LIB_WIRINGPI}
    ${LIB_BITLIB}
)
