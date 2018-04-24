cmake_minimum_required(VERSION 3.6)

project(src)

set(HOME "/home/finn/matternet")

set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_VERSION 1)
set(CMAKE_SYSTEM_PROCESSOR arm)

# set(X_COMPILER_DIR ${HOME}/compiler/gcc-linaro-4.9-2015.02-3-x86_64_arm-linux-gnueabihf)
set(X_COMPILER_DIR ${HOME}/compiler/gcc-linaro-6.3.1-2017.05-x86_64_arm-linux-gnueabihf)
# set(X_COMPILER_DIR ${HOME}/compiler/gcc-linaro-7.2.1-2017.11-x86_64_arm-linux-gnueabihf)

set(CMAKE_C_COMPILER ${X_COMPILER_DIR}/bin/arm-linux-gnueabi-gcc)
set(CMAKE_CXX_COMPILER ${X_COMPILER_DIR}/bin/arm-linux-gnueabi-g++)

set(CMAKE_SYSROOT ${HOME}/xcomp_sysroot_rpi)
set(CMAKE_FIND_ROOT_PATH ${HOME}/xcomp_sysroot_rpi)
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

# link_directories(${CMAKE_FIND_ROOT_PATH}/usr/lib/arm-linux-gnueabihf)
# include_directories(${CMAKE_FIND_ROOT_PATH}/usr/include/arm-linux-gnueabihf)

set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
find_package(Threads REQUIRED)

set(BOOST_LIBRARYDIR ${CMAKE_FIND_ROOT_PATH}/usr/lib/arm-linux-gnueabihf)
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

set(CURL_INCLUDE_DIR ${CMAKE_FIND_ROOT_PATH}/usr/include)
set(CURL_LIBRARY ${CMAKE_FIND_ROOT_PATH}/usr/lib/arm-linux-gnueabihf)
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

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -mfloat-abi=hard")

include_directories("include/slacking")
# Uncomment this line if you wish to have slacking display
# add_definitions(-DSLACKING_VERBOSE_OUTPUT=1)
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
