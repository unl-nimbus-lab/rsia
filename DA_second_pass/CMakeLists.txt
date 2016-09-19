cmake_minimum_required(VERSION 3.3)
project(phaseTwo)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/test")

include_directories(./include)


set(SOURCE_FILES src/main.cpp include/Function.h src/GlobalFunctions.cpp include/GlobalFunctions.h src/DataStructures.cpp include/IOFunctions.h src/IOFunctions.cpp include/PublishCall.h include/UpdateModel.h include/DFS.h src/UpdateModel.cpp src/DFS.cpp include/Node.h src/Node.cpp include/PublishPath.h src/PublishPath.cpp src/Function.cpp)



add_executable(phaseTwo ${SOURCE_FILES})
target_link_libraries(phaseTwo yaml-cpp)