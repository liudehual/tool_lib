#include "TCPSocket.h"
#include "Log.h"
#include <errno.h>
TCPSocket::TCPSocket():fState(kConnected)
{
	
}
TCPSocket::~TCPSocket()
{
	fprintf(stderr,"--------------> delete TCPSocket\n");
}
int TCPSocket::open()
{
	Socket::open(SOCK_STREAM);
	return 1;
}

#if defined(__WIN32__) || defined(_WIN32) || defined(_WIN32_WCE)
    static int          GetErrno();
#else
    static  int         GetErrno() { return errno; }
#endif

#if defined(__WIN32__) || defined(_WIN32) || defined(_WIN32_WCE)
int GetErrno()
{
    int winErr = ::GetLastError();
    // Convert to a POSIX errorcode. The *major* assumption is that
    // the meaning of these codes is 1-1 and each Winsock, etc, etc
    // function is equivalent in errors to the POSIX standard. This is 
    // a big assumption, but the server only checks for a small subset of
    // the real errors, on only a small number of functions, so this is probably ok.
    switch (winErr)
    {
        case ERROR_FILE_NOT_FOUND: return ENOENT;
        case ERROR_PATH_NOT_FOUND: return ENOENT;       
        case WSAEINTR:      return EINTR;
        case WSAENETRESET:  return EPIPE;
        case WSAENOTCONN:   return ENOTCONN;
        case WSAEWOULDBLOCK:return EAGAIN;
        case WSAECONNRESET: return EPIPE;
        case WSAEADDRINUSE: return EADDRINUSE;
        case WSAEMFILE:     return EMFILE;
        case WSAEINPROGRESS:return EINPROGRESS;
        case WSAEADDRNOTAVAIL: return EADDRNOTAVAIL;
        case WSAECONNABORTED: return EPIPE;
        case 0:             return 0;
        default:            return ENOTCONN;
    }
}
#endif
int TCPSocket::send(const char* inData, const unsigned int inLength, unsigned int* outLengthSent)
{
	
    int err;
    do {
       err = ::send(fSocket, inData, inLength, 0);//flags??
    } while((err == -1) && (GetErrno() == EINTR));
    if (err == -1){
        //Are there any errors that can happen if the client is connected?
        //Yes... EAGAIN. Means the socket is now flow-controleld
        int theErr = GetErrno();
        if (theErr != EAGAIN){
        	fState=kNotConnected;
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
    }while((theRecvLen == -1) && (GetErrno() == EINTR));

    if (theRecvLen == -1){
        // Are there any errors that can happen if the client is connected?
        // Yes... EAGAIN. Means the socket is now flow-controleld
        int theErr =GetErrno();
        if (theErr != EAGAIN){
        	fState=kNotConnected;
			fprintf_err("connect is wrong while read. errno is %d\n",GetErrno());
        }
        return theErr;
    }
    
    //if we get 0 bytes back from read, that means the client has disconnected.
    //Note that and return the proper error to the caller
    else if (theRecvLen == 0){
       fState=kNotConnected;
       fprintf_err("connect is reset while read. errno is %d\n",GetErrno());
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
	this->makeSocketNonBlocking(fSocket);
    /* don't forget to error check the connect()! */
    int err = ::connect(fSocket,(sockaddr *)&fRemoteAddr,sizeof(fRemoteAddr));

    if (err == 0){
        fRemoteAddr.sin_port = 0;
        fRemoteAddr.sin_addr.s_addr = 0;
        fprintf(stderr,"Connect Succeed errno %d\n",GetErrno());
        return 1;
    }
    if(GetErrno() == EINPROGRESS || GetErrno()==EAGAIN){
    	fd_set wfds;    
		struct timeval tv;    
    
		FD_ZERO(&wfds);    
		FD_SET(fSocket, &wfds);    
		/* set select() time out */    
		tv.tv_sec = 0;     
		tv.tv_usec = 500; /* 0.5s³¬Ê± */    
		int selres = select(fSocket + 1,NULL, &wfds, NULL, &tv);    
		switch (selres)    
		{    
    		case -1:    
        		printf("select error\n");    
        		return -1;    
    		case 0:    
       			printf("select time out\n");    
       			return err = -1;    
    		default: 
    			break;
		}  
		#if 1
    	int theErr = 0;
    	SOCKLEN_T len = sizeof theErr;
    	if (getsockopt(fSocket, SOL_SOCKET, SO_ERROR, (char*)&theErr, (socklen_t *)&len) < 0 || theErr != 0) {
			fprintf(stderr,"getsockopt Connect Error errno %d\n",GetErrno());
			return -1;		
    	}
    	#endif
    	fprintf(stderr,"is Connecting \n");
    }
    fprintf(stderr,"Err %d GetErrno() %d \n",err,GetErrno());
    return 1;
}
