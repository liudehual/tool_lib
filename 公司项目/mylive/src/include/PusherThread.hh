#ifndef __PUSHER_THREAD_HH__
#define __PUSHER_THREAD_HH__


#include "liveMedia.hh"
#include "BasicUsageEnvironment.hh"
#include <pthread.h>
#include <unistd.h>

#include "PusherByteStreamSource.hh"

class Frame;
class CacheManager;
class PusherThread
{
	public:
		PusherThread();
		~PusherThread();

		// kill self
		void kill();
		
		//init thread
		int initPusherThread();
		
		int start();
		int Run();
		static void * threadRun(void *args);
		static void afterPlaying(void *clientData);
		int connectToServer(char *sIp,
							 unsigned short sPort,
							 char *remoteSDPFileName,
							 char *uName,
							 char *uPassword);

		CacheManager *getManager(){return manager;}

		int writeData(Frame *tFrame);
		int readData(Frame *tFrame);

		void setThreadState(int tState){fState=tState;}
	private:
		pthread_t threadId;
		UsageEnvironment* env;
		DarwinInjector* injector;
		struct in_addr dummyDestAddress;
		CacheManager *manager;
		
		//video
		RTPSink* videoSink;
		PusherByteStreamSource *videoStreamSource;
		Groupsock *rtpGroupsockVideo;
  		Groupsock *rtcpGroupsockVideo;
  	    RTCPInstance* videoRTCP;

		//audio
		RTPSink* audioSink;
		PusherByteStreamSource *audioStreamSource;
 		Groupsock *rtpGroupsockAudio;
  		Groupsock *rtcpGroupsockAudio;
  		RTCPInstance* audioRTCP;
  		
  		// Create (and start) a 'RTCP instance' for this RTP sink:
  		unsigned estimatedSessionBandwidthVideo; // in kbps; for RTCP b/w share
  		unsigned maxCNAMElen;
  		unsigned char *CNAME;
  		char serverIp[64];
  		unsigned short serverPort;
  		char remoteStreamName[64];

		// Create (and start) a 'RTCP instance' for this RTP sink:
  		unsigned estimatedSessionBandwidthAudio; // in kbps; for RTCP b/w share
  		unsigned maxAudioCNAMElen;
  		unsigned char *audioCNAME;
  		
  		char userName[64];
  		char userPassword[64];

  		char fWatchVariable;

		int fState;   /* 标志当前正在处理的是视频还是音频 */
};

#endif

