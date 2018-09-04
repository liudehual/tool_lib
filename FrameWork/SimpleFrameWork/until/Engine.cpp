#include "Engine.h"
#include <stdio.h>
#include "EventJob.h"
#include "Socket.h"
#include "Until.h"
#include "TimeoutJob.h"
#include "TimeManager.h"
#include "Log.h"


Engine::Engine():fMaxNumSockets(0),runFlags(false)
{
  FD_ZERO(&fReadSet);
  FD_ZERO(&fWriteSet);
  FD_ZERO(&fExceptionSet);
  mManager=new TimeManager;
}
Engine::~Engine()
{	
	fprintf_debug("delete Engine\n");

	delete mManager;
}
void Engine::start()
{
	fprintf_debug("%s %d %s\n",__FUNCTION__,__LINE__,"Start Loop");
	runFlags=true;
	dispatch();
}
void Engine::end()
{
	runFlags=false;
}
void Engine::dispatch()
{
	while(true){

		if(!runFlags){
			break;
		}
		
		mManager->Processor();

  		fd_set readSet = fReadSet;
  		fd_set writeSet = fWriteSet;
  		fd_set exceptionSet = fExceptionSet;
		
  		struct timeval timeToDelay;
  		timeToDelay.tv_sec=0;
  		timeToDelay.tv_usec=10*1000;//10 ms delay

  		int selectResult = select(fMaxNumSockets, &readSet, &writeSet, &exceptionSet, &timeToDelay);
		//	fprintf(stderr,"select() selectResult %d\n",selectResult);
	  	if (selectResult < 0) { //error
	  		fprintf(stderr,"selectResult %d\n",selectResult);
			#if defined(__WIN32__) || defined(_WIN32)
    			int err = WSAGetLastError();
			fprintf_debug("err %d\n",err);
    		if (err == WSAEINVAL && readSet.fd_count == 0) {
      			err = EINTR;
      		int dummySocketNum = socket(AF_INET, SOCK_DGRAM, 0);
      		FD_SET((unsigned)dummySocketNum, &fReadSet);
    	}
    		if (err != EINTR) {
		#else
    		if (errno != EINTR && errno != EAGAIN) {
		#endif
		#if ! (defined(__WIN32__) || defined(_WIN32))
			perror("EventLoop::loop(): select() fails");
		#endif
       	}
			#if defined(__WIN32__) || defined(_WIN32)
       			#else
			fprintf_err("select() failer errno is %d Result %d\n",errno,selectResult);
		#endif
      	}

      	if(selectResult>0){
			for(ObjectQueueIter it(&eventQueue);
							!(it.IsDone());it.Next()){
				ObjectQueueElem *elem=it.GetCurrent();
			    EventJob*theJob=(EventJob *)(elem->GetEnclosingObject());
				int sock=theJob->getSocket()->getSocketFD();
				if (FD_ISSET(sock,&readSet)
						&&FD_ISSET(sock,&fReadSet)){
					if((theJob->Processor())==0){
						delEvent(theJob);
						break;
					}
				}
    			if (FD_ISSET(sock,&writeSet)
    					&&FD_ISSET(sock,&fWriteSet)){
    									fprintf_debug("========================= SocketFd %d\n",sock);

					if(!(theJob->Processor())==0){
						delEvent(theJob);
						break;
					}
    			}
    			if (FD_ISSET(sock,&exceptionSet)
    					&&FD_ISSET(sock,&fExceptionSet)){
    									fprintf_debug("========================= SocketFd %d\n",sock);

					if(!(theJob->Processor())==0){
						delEvent(theJob);
						break;
					}
    			}
			}
		}
  }
}
int Engine::addEvent(void *object,unsigned int conditionSet)
{
	if(object==NULL){
		return 0;
	}

	EventJob *theJob=(EventJob *)object;
	int sockFd=theJob->getSocket()->getSocketFD();

	if(sockFd<0){
	  return 0;
	}

	FD_CLR((unsigned)sockFd,&fReadSet);
  	FD_CLR((unsigned)sockFd,&fWriteSet);
  	FD_CLR((unsigned)sockFd,&fExceptionSet);

	if (conditionSet&SOCKET_READABLE) FD_SET((unsigned)sockFd,&fReadSet);
    if (conditionSet&SOCKET_WRITABLE) FD_SET((unsigned)sockFd,&fWriteSet);
    if (conditionSet&SOCKET_EXCEPTION) FD_SET((unsigned)sockFd,&fExceptionSet);

	if (sockFd+1 >= fMaxNumSockets){
      fMaxNumSockets = sockFd+1;
    }
	eventQueue.EnQueue(theJob->getJobElem());
	fprintf_debug("%s %d sockFd %d fMaxNumSockets %d %s\n",__FUNCTION__,__LINE__,sockFd,fMaxNumSockets,"[[[[ add Event is succeed ]]]]");
	return 1;
}
int Engine::delEvent(void *object,unsigned int conditionSet)
{

	fprintf_debug("--------------> delEvent\n");

	if(object==NULL){
		return 0;
	}

	EventJob *theJob=(EventJob *)object;
	int sockFd=theJob->getSocket()->getSocketFD();

	if (sockFd < 0){
		return 0;
	} 

   	if (FD_ISSET(sockFd, &fReadSet)){FD_CLR((unsigned)sockFd, &fReadSet); }
   	if (FD_ISSET(sockFd, &fWriteSet)){FD_CLR((unsigned)sockFd, &fWriteSet);}
   	if (FD_ISSET(sockFd, &fExceptionSet)){FD_CLR((unsigned)sockFd, &fExceptionSet);}

   	fprintf_debug("delEvent sockFd %d fMaxNumSockets %d\n",sockFd,fMaxNumSockets);

   	if (sockFd+1 == fMaxNumSockets) {
    	--fMaxNumSockets;
   	}
	eventQueue.Remove(theJob->getJobElem());
	/*Job?????????*/
	delete theJob;
   	#if 0
	ObjectQueueIter it(&eventQueue);
	for(;!(it.IsDone());it.Next()){
		ObjectQueueElem *elem=it.GetCurrent();
		if(object==(elem->GetEnclosingObject())){
			elem->SetEnclosingObject(NULL);
			elem->Remove();
			eventQueue.Remove(elem);
			//it.Reset();
			Task *theTask=(Task *)(elem->GetEnclosingObject());
			::close(sockFd);
		//	delete elem;
			//		fprintf(stderr,SIMPLE_RTSP"delete Rtsp Session is succeed\n");
			//delete theTask;
		}
	}
	#endif
	return 1;
}
int Engine::addElemToTimeoutQueue(void *object)
{	
	if(object==NULL){
		return 0;
	}
	Job *theJob=(Job *)object;
	
	mManager->addElem(theJob->getJobElem());
	return 1;
}
int Engine::delElemFromTimeoutQueue(void *object)
{
	if(object==NULL){
		return 0;
	}
	mManager->delElem(object);
	#if 0
	ObjectQueueIter it(&timeoutQueue);
	for(;!(it.IsDone());it.Next()){
		ObjectQueueElem *elem=it.GetCurrent();
		if(object==(elem->GetEnclosingObject())){
			eventQueue.Remove(elem);
			Task *theTask=(Task *)(elem->GetEnclosingObject());
			delete elem;
			delete theTask;
		}
	}
	#endif
	return 1;
}
