#include "PusherThread.hh"
#include "FramesManager.hh"
#include "PusherAPI.hh"
PusherThread::PusherThread():env(NULL),
							 videoSink(NULL),
							 injector(NULL),
							 rtpGroupsockVideo(NULL),
							 rtcpGroupsockVideo(NULL),
							 videoRTCP(NULL),
							 videoStreamSource(NULL),
							 fWatchVariable(0),
							 audioSink(NULL),
							 rtpGroupsockAudio(NULL),
							 rtcpGroupsockAudio(NULL),
							 audioRTCP(NULL),
							 audioStreamSource(NULL),
							 manager(new CacheManager(2*1024*1024,3)),
							 fState(DO_NOTHING)
{
	memset(serverIp,'\0',32);
	serverPort=0;

	memset(userName,'\0',64);
	memset(userPassword,'\0',64);

	memset(remoteStreamName,'\0',64);
}

PusherThread::~PusherThread()
{
}

int PusherThread::start()
{
    pthread_attr_t  attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	pthread_create(&threadId,&attr,PusherThread::threadRun,this);
}

int PusherThread::Run()
{
	env->taskScheduler().doEventLoop(&fWatchVariable); // does not return
}

void * PusherThread::threadRun(void *args)
{
	PusherThread *thread=(PusherThread *)args;
	thread->Run();
	return thread;
}
void PusherThread::afterPlaying(void *clientData)
{
	PusherThread *thread=(PusherThread *)clientData;
	return;	
}

int PusherThread::initPusherThread()
{
  	TaskScheduler* scheduler = BasicTaskScheduler::createNew();
  	env = BasicUsageEnvironment::createNew(*scheduler);
  	if(!env){
		return 0;
  	}
  	return 1;
}
int PusherThread::connectToServer(char *sIp,
							 unsigned short sPort,
							 char *remoteSDPFileName,
							 char *uName,
							 char *uPassword)
{	
	static char *programName="myPusher";

	memcpy(serverIp,sIp,strlen(sIp));
	serverPort=sPort;

	memcpy(userName,uName,strlen(uName));
	memcpy(userPassword,uPassword,strlen(uPassword));

	if(strlen(remoteSDPFileName)>64){
		memcpy(remoteStreamName,remoteSDPFileName,64);
	}else{
		memcpy(remoteStreamName,remoteSDPFileName,strlen(remoteSDPFileName));
	}
	
	injector = DarwinInjector::createNew(*env, programName);

	/* VIDEO */
	dummyDestAddress.s_addr = 0;

	rtpGroupsockVideo=new Groupsock(*env, dummyDestAddress, 0, 0);
	rtcpGroupsockVideo=new Groupsock(*env, dummyDestAddress, 0, 0);

	OutPacketBuffer::maxSize=2000000;
  	videoSink = H264VideoRTPSink::createNew(*env, rtpGroupsockVideo,96);


   	estimatedSessionBandwidthVideo = 50000; // in kbps; for RTCP b/w share
    maxCNAMElen = 10000;
   	CNAME=new unsigned char[maxCNAMElen+1];
  	gethostname((char*)CNAME, maxCNAMElen);
  	CNAME[maxCNAMElen] = '\0';

  	videoRTCP =RTCPInstance::createNew(*env, rtcpGroupsockVideo,
			      							estimatedSessionBandwidthVideo, 
			      							CNAME,
			      							videoSink, 
			      							NULL);
			      							
   	injector->addStream(videoSink, videoRTCP);
   	/* VIDEO END */

	/* AUDIO  */
	dummyDestAddress.s_addr = 0;

	estimatedSessionBandwidthAudio = 160; // in kbps; for RTCP b/w share
    maxAudioCNAMElen = 10000;
   	audioCNAME=new unsigned char[maxAudioCNAMElen+1];
  	gethostname((char*)audioCNAME, maxAudioCNAMElen);
  	audioCNAME[maxAudioCNAMElen] = '\0';
	
	rtpGroupsockAudio=new Groupsock(*env, dummyDestAddress, 0, 0);
	rtcpGroupsockAudio=new Groupsock(*env, dummyDestAddress, 0, 0);

	/* 使用默认参数 */
	audioSink=MPEG4GenericRTPSink::createNew(*env, rtpGroupsockAudio,
													96,
													44100,
													"audio", 
													"AAC-hbr", 
													"",
													2);

  	audioRTCP =RTCPInstance::createNew(*env, rtcpGroupsockAudio,
			      							estimatedSessionBandwidthAudio, 
			      							audioCNAME,
			      							audioSink, 
			      							NULL);
			      							
	injector->addStream(audioSink, audioRTCP);
	/* AUDIO END */
	
  	if (!injector->setDestination(serverIp, remoteStreamName,
				programName, "Pusher Streaming Media")) {
    	*env << "injector->setDestination() failed: "
	 	<< env->getResultMsg() << "\n";
    	return 0;
  	}

 	videoStreamSource =PusherByteStreamSource::createNew(*env,2*1024*1024,VIDEO_TYPE);
 	Boolean videoStartPlay= videoSink->startPlaying(*videoStreamSource, afterPlaying, this);

	audioStreamSource=PusherByteStreamSource::createNew(*env, 512*1024,AUDIO_TYPE);
	Boolean audioStartPlay=audioSink->startPlaying(*audioStreamSource,afterPlaying,this);

 	this->start();

 	return 1;
}
void PusherThread::kill()
{
	fWatchVariable=1;
}

int PusherThread::writeData(Frame *tFrame)
{
	manager->writeData(tFrame);
}

int PusherThread::readData(Frame *tFrame)
{   
	#if 1
	if(fState!=DO_NOTHING){
		return -1;    /* 当前忙不允许读取数据 */
	}
	/* 探测一下 */
	int type=manager->findMediaType();
	if(tFrame->type!=type){
		return 2;
	}
	
	Frame f;
	f.bufferLen=0;
	f.buffer=NULL;
	f.type=0;
	
	int flags=manager->readData(&f);
	//fprintf(stderr,"flags %d \n",flags);
	if(f.bufferLen==0){
	//	fprintf(stderr,"not find frame \n");
		return 0; /* 没有找到数据 */
	}
	
	//fprintf(stderr,"len %d f.type %d tFrame->type %d \n",f.bufferLen,f.type,tFrame->type);

	tFrame->buffer=f.buffer;
	tFrame->bufferLen=f.bufferLen;
	//fprintf(stderr,"bufferLen %d \n",bufferLen);
	//delete f;
	return 1;

	#endif
}
