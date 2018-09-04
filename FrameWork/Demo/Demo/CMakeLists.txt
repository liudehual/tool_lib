INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/CommonUtilitiesLib-linux)
project(demo)
set(SRC_LIST
	main.cpp
)

add_executable(demo ${SRC_LIST})
target_link_libraries(demo commonUntilsLib)
