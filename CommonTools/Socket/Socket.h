/*

*/
#ifndef __SOCKET_H__
#define __SOCKET_H__
#include "NetCommon.h"
#include <stdio.h>
#include <stdlib.h>

class Socket
{
	protected:
		Socket();
	public:
		virtual ~Socket();
		int getSocketFD(){return fSocket;}
		int open(int theType);
		void InitNonBlocking(int inFileDesc);	
		
		unsigned getBufferSize(int bufOptName,int socket); 
		unsigned getSendBufferSize(); 
		unsigned getReceiveBufferSize(); 

		unsigned setBufferTo(int bufOptName,int socket, unsigned requestedSize);
		unsigned setSendBufferTo(unsigned requestedSize); 
		unsigned setReceiveBufferTo(unsigned requestedSize); 

		unsigned increaseBufferTo(int bufOptName,int socket, unsigned requestedSize); 
		unsigned increaseSendBufferTo(unsigned requestedSize);
		unsigned increaseReceiveBufferTo(unsigned requestedSize);

		bool makeSocketNonBlocking(int sock);
		bool makeSocketBlocking(int sock); 

		int bind(unsigned int addr, unsigned short port);

		unsigned int strIpToUInt32(char * ip);
		void UInt32ToStrIp(unsigned int uIntIp,char *strIp);

		char *getRemoteStrIp();
		char *getLocalStrIp();
		
		unsigned short getRemotePort();
		unsigned short getLocalPort();

		void setRemoteStrIp(char *ip);
		void setLocalStrIp(char *ip);
		void setRemotePort(unsigned short port);
		void setLocalPort(unsigned short port);
	protected:
		int fSocket;
		struct sockaddr_in  fLocalAddr;

	    //Զ��IP���˿�
	 	char remoteIp[32];
	  	unsigned short remotePort;

		//����IP���˿�
	 	char localIp[32];
	  	unsigned short localPort;
};

#endif
