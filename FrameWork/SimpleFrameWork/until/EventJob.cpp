#include "EventJob.h"
#include "Log.h"
#include "Socket.h"
EventJob::EventJob(Socket *tSocket):fSocket(tSocket)
{
}
EventJob::~EventJob()
{
	fprintf_debug("-----------------> ================ Close EventJob\n");
	if(fSocket!=NULL){
		delete fSocket;
		fSocket=NULL;
	}
}
Socket *EventJob::getSocket()
{
	return fSocket;
}
int EventJob::Signal(unsigned int conditionSet)
{
	Engine *eng=getEngine();
	eng->addEvent((void *)this,conditionSet);
	return 1;
}