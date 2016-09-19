cmake_minimum_required(VERSION 3.5)
project(roslaunch_reader)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

include_directories(include)

set(SOURCE_FILES src/main.cpp src/roslaunch_xml.cpp src/node_visiters.cpp src/helper_functions.cpp)
add_executable(roslaunch_reader ${SOURCE_FILES})
target_link_libraries(roslaunch_reader pugixml)
target_link_libraries(roslaunch_reader yaml-cpp)