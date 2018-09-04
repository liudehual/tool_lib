#ifndef __TCP_LISTENER_H__
#define __TCP_LISTENER_H__
#include "EventJob.h"
class TCPSocket;
class TCPListener:public EventJob
{
	public:
		TCPListener();
		~TCPListener();
		virtual int Processor();
		virtual EventJob *GetSessionJob(TCPSocket** outSocket)=0;
		int Listen(int queueLength);
		int Initialize(unsigned int addr, unsigned short port);
};
#endif
