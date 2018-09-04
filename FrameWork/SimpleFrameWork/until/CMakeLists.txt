project(until)
include_directories(.)
set(CMAKE_BUILD_TYPE Release)
set(SRC_LIST
	TCPSocket.cpp
	UDPSocket.cpp
	Socket.cpp
	Object.cpp
	Engine.cpp
	Job.cpp
	ObjectQueue.cpp
	Until.cpp
	TimeoutJob.cpp
	EventJob.cpp
	TimeManager.cpp
	SocketHelper.cpp
	StrPtr.cpp
	TCPListener.cpp
	ServerEventJob.cpp
	ClientEventJob.cpp
)
add_library(until ${SRC_LIST})
if(LINUX)
TARGET_LINK_LIBRARIES(until pthread)          
endif()
