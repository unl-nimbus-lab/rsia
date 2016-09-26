cmake_minimum_required(VERSION 3.3)
project(DA_second_pass)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

include_directories(./include)

set(SOURCE_FILES
		src/main.cpp
		src/GlobalFunctions.cpp
		src/DataStructures.cpp
		src/IOFunctions.cpp
		src/UpdateModel.cpp
		src/DFS.cpp
		src/Node.cpp
		src/PublishPath.cpp
		src/Function.cpp
)

add_executable(DA_second_pass ${SOURCE_FILES})
target_link_libraries(DA_second_pass yaml-cpp)