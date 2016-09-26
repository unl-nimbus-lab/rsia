cmake_minimum_required(VERSION 3.5)
project(IA_tool)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 ")
include_directories(./include)
set(SOURCE_FILES
		src/main.cpp
		src/ROSNode.cpp
		src/common.cpp
		src/ROSTopic.cpp
		src/ROSEdge.cpp
		src/FileOperations.cpp
		src/Analysis.cpp
		src/DFS.cpp
		)


add_executable(GraphTraversal ${SOURCE_FILES})
target_link_libraries(GraphTraversal
		yaml-cpp)

#add_executable(TableGenerator Analysis/TableGenerator.cpp)
#target_link_libraries(TableGenerator yaml-cpp)


#add_executable(NewTableGenerator Analysis/NewTableGenerator.cpp)
#target_link_libraries(NewTableGenerator yaml-cpp)

#add_executable(ListGen ListGenerator/main.cpp)
#add_executable(MismatchCounter  Analysis/MismatchCounter.cpp)
#add_executable(DotWritter src/DotWritter.cpp
#		src/ROSNode.cpp
#		src/common.cpp
#		src/ROSTopic.cpp
#		src/ROSEdge.cpp
#		src/FileOperations.cpp
#		src/Analysis.cpp
#		src/DFS.cpp)
#target_link_libraries(DotWritter
#		yaml-cpp)

#add_dependencies(GraphTraversal
#		yaml-cpp)
