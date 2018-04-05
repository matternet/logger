cmake_minimum_required(VERSION 2.8)

include_directories("include/slacking")

project(src)

# Uncomment this line if you wish to have slacking display
# add_definitions(-DSLACKING_VERBOSE_OUTPUT=1)

find_package(CURL)
include_directories(${CURL_INCLUDE_DIRS})

option(CURL_STATIC_LINKING
""
  OFF
  )

if(CURL_STATIC_LINKING)
add_definitions(-DCURL_STATICLIB)
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -lwiringPi")

if ("${CMAKE_MAJOR_VERSION}${CMAKE_MINOR_VERSION}" LESS 31)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif()

include_directories("src")

set (SOURCES_SRC
    src/main.cpp
)

add_executable(temp-slack-logger ${SOURCES_SRC})
set_property(TARGET temp-slack-logger PROPERTY CXX_STANDARD 11)
set_property(TARGET temp-slack-logger PROPERTY CXX_STANDARD_REQUIRED ON)
target_link_libraries(temp-slack-logger ${CURL_LIBRARIES})
