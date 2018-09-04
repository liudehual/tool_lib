#include "PusherAPI.hh"

#include "liveMedia.hh"
#include "BasicUsageEnvironment.hh"
#include "PusherThread.hh"
#include "OSMutex.hh"
#include "Media.hh"
static PusherThread *thread=NULL;

int pusher_init()
{
	pusher_deinit();
	Medium::fThread=thread=new PusherThread();
	if(!thread){
		return 0;
	}
	
  	if(!thread->initPusherThread()){
  		fprintf(stderr,"something is wrong while init pusherThread \n");
		return 0;
  	}
	return 1;
}

int pusher_connect(char *serverAddr,
						unsigned short serverPort,
						char *remoteSDPFileName,
						char *userName,
						char *userPassword)
{
	if(serverAddr==NULL || strlen(serverAddr)<7
		|| userName==NULL
		|| userPassword==NULL
		|| remoteSDPFileName==NULL || strlen(remoteSDPFileName)<5){
		return 0;
	}
	if(!thread){
		return 0;
	}

	int connectState=thread->connectToServer(serverAddr,
								serverPort,
								remoteSDPFileName,
								userName,
								userPassword);
	if(!connectState){
		return 0;
	}
	return 1;
}

int pusher_pushFrame(Frame tFrame)
{
	static OSMutex mutex;
	mutex.Lock();
	
	
	//fprintf(stderr," pusher_pushFrame f.bufferLen %d f.type %d \n",f.bufferLen,f.type);

	thread->writeData(&tFrame);
	
	mutex.Unlock();
	return 1;
}

int pusher_deinit()
{
	if(thread){
		thread->kill();
		usleep(1000*20);
		delete thread;
		thread=NULL;
	}
	return 1;
}
int getAudioSleepTime(int fSamplingFrequency)
{
   
    return (1024/*samples-per-frame*/*1000000) / fSamplingFrequency/*samples-per-second*/;
}
