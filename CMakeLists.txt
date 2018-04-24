cmake_minimum_required(VERSION 2.8)

include_directories("include/slacking")

project(src)

# Uncomment this line if you wish to have slacking display
# add_definitions(-DSLACKING_VERBOSE_OUTPUT=1)

find_package(Threads REQUIRED)
find_package(Boost COMPONENTS
    program_options # Used for command line arguments
    filesystem # Used to open/close files
REQUIRED)
set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED OFF)

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
    set (SOURCES_TEMPERATURE
        src/temperature-logger.cpp
    )
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

include_directories("src")

set (SOURCES_SRC
    src/main.cpp
    src/serial-logger.cpp
    ${SOURCES_TEMPERATURE}
)

add_executable(logger ${SOURCES_SRC})
target_link_libraries(logger
    ${CMAKE_THREAD_LIBS_INIT}
    ${CURL_LIBRARIES}
    ${Boost_LIBRARIES}
)
