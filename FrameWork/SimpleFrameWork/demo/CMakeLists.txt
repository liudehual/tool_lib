INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/until)

project(testUntil)
include_directories(.)

set(SOURCE_LIST
	main.cpp
	getopt.cpp
)
ADD_EXECUTABLE(testUntil                                    
     ${SOURCE_LIST}                                       
)  

if(LINUX)
	set(LIB until)
else()
	set(LIB until)
endif()

 
TARGET_LINK_LIBRARIES(testUntil ${LIB})          
                                                                                               
add_test(DemoTest testUntil -s)
