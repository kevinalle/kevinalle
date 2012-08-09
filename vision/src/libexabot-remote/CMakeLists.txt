cmake_minimum_required(VERSION 2.8)

find_package(Boost COMPONENTS system REQUIRED)

add_library(exabot-remote SHARED libexabot-remote.cpp)
include_directories(${PROJECT_SOURCE_DIR})
target_link_libraries(exabot-remote ${Boost_SYSTEM_LIBRARY})

add_executable(test test.cpp)
include_directories(${PROJECT_SOURCE_DIR})
target_link_libraries(test exabot-remote)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -g")
