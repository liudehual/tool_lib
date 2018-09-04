project(CommonTools)
include_directories(.)
include_directories(Thread)
include_directories(Socket)
set(SRC_LIST
	base64.cpp
	daemon.cpp
	getopt.cpp
	MakeDir.cpp
	md5.cpp
	random.cpp
	test.cpp
	StrPtr.cpp
	OSQueue.cpp
	OSHeap.cpp
	OSCodeFragment.cpp

	GetNetInterfaceInfo.cpp
	
	Thread/OSThread.cpp
	Thread/OSCond.cpp
	Thread/OSMutex.cpp
	
	Socket/Socket.cpp
	Socket/SocketHelper.cpp
	Socket/TCPSocket.cpp
	Socket/UDPSocket.cpp
	
	
)
add_executable(test ${SRC_LIST})
# dlopen dlsym dlclose 函数需要 ld 库文件 
	target_link_libraries(test dl)
	target_link_libraries(test pthread rt)
