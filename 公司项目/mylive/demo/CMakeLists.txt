include_directories(${PROJECT_SOURCE_DIR}/src/include)
project(myPusherDemo)
cmake_minimum_required (VERSION 2.8)

include_directories(.)
set(SRC_LIST
	demo.cpp	
)
add_executable(myPusherDemo ${SRC_LIST})
target_link_libraries(myPusherDemo myPusher)
