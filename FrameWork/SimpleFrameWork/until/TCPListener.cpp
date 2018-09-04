#include "TCPListener.h"
#include "NetCommon.h"
#include "TCPSocket.h"
#include "Log.h"
TCPListener::TCPListener():EventJob(new TCPSocket())
{

}
TCPListener::~TCPListener()
{

}
int TCPListener::Processor()
{
	 struct sockaddr_in addr;
#if defined(__WIN32__) || defined(_WIN32)	
    int size = sizeof(addr);
#else
    socklen_t size = sizeof(addr);
#endif

    EventJob* theJob = NULL;
    TCPSocket* theSocket = NULL;
    //fSocket data member of TCPSocket.
	int osSocket = ::accept(getSocket()->getSocketFD(), (struct sockaddr*)&addr,&size);
	//fprintf_debug("Accept New Client %d\n",osSocket);
	if(osSocket<0){
		//fprintf_err("%d osSocket %d errno %d size %d\n",getSocket()->getSocketFD(),osSocket,errno,size);
		return 1;
	}
	fprintf_debug("osSocket %d\n",osSocket);
	theJob = this->GetSessionJob(&theSocket);

	//set options on the socket
    //we are a server, always disable nagle algorithm
    int one = 1;
       
    int err = ::setsockopt(osSocket, SOL_SOCKET, SO_KEEPALIVE, (char*)&one, sizeof(int));
    
    int sndBufSize = 96L * 1024L;
    err = ::setsockopt(osSocket, SOL_SOCKET, SO_SNDBUF, (char*)&sndBufSize, sizeof(int));

	theSocket->setSocket(osSocket);
	theSocket->makeSocketNonBlocking(osSocket);

	theJob->Signal(SOCKET_READABLE);
	return 1;
}
int TCPListener::Listen(int queueLength)
{
    int err = ::listen(getSocket()->getSocketFD(), queueLength);
    if (err != 0)
        return errno;
    return 0;
}
int TCPListener::Initialize(unsigned int addr, unsigned short port)
{
    int err = ((TCPSocket *)getSocket())->open();
    if (0 != err) do
    {   
        // set SO_REUSEADDR socket option before calling bind.
#if !defined(__WIN32__) && !defined(_WIN32)
        // this causes problems on NT (multiple processes can bind simultaneously),
        // so don't do it on NT.
        getSocket()->ReuseAddr();
#endif

        err = ((TCPSocket *)getSocket())->bind(addr, port);
        if (err == 0) break; // don't assert this is just a port already in use.

        //
        // Unfortunately we need to advertise a big buffer because our TCP sockets
        // can be used for incoming broadcast data. This could force the server
        // to run out of memory faster if it gets bogged down, but it is unavoidable.
        ((TCPSocket *)getSocket())->setReceiveBufferTo(96 * 1024); 

        err = this->Listen(60);
        if (err != 0) break;
        
    } while (false);
    
    return err;
}
