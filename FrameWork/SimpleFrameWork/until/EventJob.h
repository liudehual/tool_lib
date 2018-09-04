#ifndef __EVENT_JOB_H__
#define __EVENT_JOB_H__
#include "Job.h"

class Socket;
class EventJob:public Job
{
	
	protected:
		EventJob(Socket *tSocket);
	public:
		virtual ~EventJob();
		
		virtual Socket *getSocket();
		virtual int Signal(unsigned int conditionSet);
	private:
		Socket *fSocket;
};
#endif
