#ifndef __CLIENT_EVENT_JOB_H__
#define __CLIENT_EVENT_JOB_H__
#include "EventJob.h"
class ClientEventJob:public EventJob
{
	public:
		ClientEventJob();
		~ClientEventJob();
		int Initialize(unsigned int addr, unsigned short port);
};
#endif
