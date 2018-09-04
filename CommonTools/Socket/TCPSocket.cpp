#include "TCPSocket.h"
#include "Log.h"

TCPSocket::TCPSocket()
{
	open(SOCK_STREAM);
}
TCPSocket::~TCPSocket()
{
}

int TCPSocket::send(const char* inData, const unsigned int inLength, unsigned int* outLengthSent)
{
    int err;
    do {
       err = ::send(fSocket, inData, inLength, 0);//flags??
    } while((err == -1) && (errno == EINTR));
    if (err == -1){
        //Are there any errors that can happen if the client is connected?
        //Yes... EAGAIN. Means the socket is now flow-controleld
        int theErr = errno;
        if (theErr != EAGAIN){
			fprintf_err("connect is reset.errno is %d\n",errno);
        }
        return theErr;
    }
    *outLengthSent = err;
    return err;
	
}
int TCPSocket::read(void *buffer, const unsigned int length, unsigned int *outRecvLenP)
{
    //int theRecvLen = ::recv(fFileDesc, buffer, length, 0);//flags??
    int theRecvLen;
    do{
       theRecvLen = ::recv(fSocket, (char*)buffer, length, 0);//flags??
    }while((theRecvLen == -1) && (errno == EINTR));

    if (theRecvLen == -1){
        // Are there any errors that can happen if the client is connected?
        // Yes... EAGAIN. Means the socket is now flow-controleld
        int theErr =errno;
        if (theErr != EAGAIN){
			fprintf_err("connect is wrong while read. errno is %d\n",errno);
        }
        return theErr;
    }
    
    //if we get 0 bytes back from read, that means the client has disconnected.
    //Note that and return the proper error to the caller
    else if (theRecvLen == 0){
       fprintf_err("connect is reset while read. errno is %d\n",errno);
       return theRecvLen;
    }
    *outRecvLenP = (unsigned int)theRecvLen;
    return theRecvLen;
}

int  TCPSocket::connect(unsigned int inRemoteAddr,unsigned short inRemotePort)
{
    ::memset(&fRemoteAddr, 0, sizeof(fRemoteAddr));
    fRemoteAddr.sin_family = AF_INET;        /* host byte order */
    fRemoteAddr.sin_port = htons(inRemotePort); /* short, network byte order */
    fRemoteAddr.sin_addr.s_addr = htonl(inRemoteAddr);

    /* don't forget to error check the connect()! */
    int err = ::connect(fSocket,(sockaddr *)&fRemoteAddr,sizeof(fRemoteAddr));
    
    if (err == -1){
        fRemoteAddr.sin_port = 0;
        fRemoteAddr.sin_addr.s_addr = 0;
        return errno;
    }
    
    return err;

}
