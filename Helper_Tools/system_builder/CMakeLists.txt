cmake_minimum_required(VERSION 3.5)
project(software_builder)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

include_directories(include)

set(SOURCE_FILES src/main.cpp)
add_executable(software_builder ${SOURCE_FILES})
target_link_libraries(software_builder yaml-cpp)