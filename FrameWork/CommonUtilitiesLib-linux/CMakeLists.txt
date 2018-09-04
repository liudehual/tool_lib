project(commonUntilsLib)
include_directories(.)
set(SRC_LIST
	atomic.cpp
	DateTranslator.cpp
	EventContext.cpp
	IdleTask.cpp
	OSCond.cpp
	OS.cpp
	OSHeap.cpp
	OSMutex.cpp
	OSMutexRW.cpp
	OSQueue.cpp
	OSRef.cpp
	OSThread.cpp
	MyAssert.cpp
	Socket.cpp
	SocketUtils.cpp
	StringFormatter.cpp
	StringParser.cpp
	StringTranslator.cpp
	StrPtrLen.cpp
	Task.cpp
	TCPListenerSocket.cpp
	TCPSocket.cpp
	TimeoutTask.cpp
	UDPSocket.cpp
	UDPSocketPool.cpp
	InternalStdLib.cpp
	UDPDemuxer.cpp
	ev.cpp
)

add_library(commonUntilsLib STATIC ${SRC_LIST})
target_link_libraries(commonUntilsLib pthread)
