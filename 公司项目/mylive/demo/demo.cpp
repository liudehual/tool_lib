#if 0
#include "liveMedia.hh"
#include "BasicUsageEnvironment.hh"

UsageEnvironment* env;
char const* inputFileName = "D1.h264";
char const* remoteStreamName = "test23.sdp"; // the stream name, as served by the DSS
FramedSource* videoSource;
RTPSink* videoSink;

char const* programName;

// To stream *only* MPEG "I" frames (e.g., to reduce network bandwidth),
// change the following "False" to "True":
Boolean iFramesOnly = False;

void usage() {
  *env << "usage: " << programName
       << " <Darwin Streaming Server name or IP address>\n";
  exit(1);
}

void play(); // forward

int main(int argc, char** argv) {
  // Begin by setting up our usage environment:
  TaskScheduler* scheduler = BasicTaskScheduler::createNew();
  env = BasicUsageEnvironment::createNew(*scheduler);

  // Parse command-line arguments:
  programName = argv[0];
  if (argc != 2) usage();
  char const* dssNameOrAddress = argv[1];

  // Create a 'Darwin injector' object:
  DarwinInjector* injector = DarwinInjector::createNew(*env, programName);

  // Create 'groupsocks' for RTP and RTCP.
  // (Note: Because we will actually be streaming through a remote Darwin server,
  // via TCP, we just use dummy destination addresses, port numbers, and TTLs here.)
  struct in_addr dummyDestAddress;
  dummyDestAddress.s_addr = 0;
  ////////// VIDEO //////////
  // Create 'groupsocks' for RTP and RTCP.
  // (Note: Because we will actually be streaming through a remote Darwin server,
  // via TCP, we just use dummy destination addresses, port numbers, and TTLs here.)
  Groupsock rtpGroupsockVideo(*env, dummyDestAddress, 0, 0);
  Groupsock rtcpGroupsockVideo(*env, dummyDestAddress, 0, 0);

  // Create a 'H264 Video RTP' sink from the RTP 'groupsock':
  videoSink = H264VideoRTPSink::createNew(*env, &rtpGroupsockVideo,96);

  // Create (and start) a 'RTCP instance' for this RTP sink:
  const unsigned estimatedSessionBandwidthVideo = 5000; // in kbps; for RTCP b/w share
  const unsigned maxCNAMElen = 1000;
  unsigned char CNAME[maxCNAMElen+1];
  gethostname((char*)CNAME, maxCNAMElen);
  CNAME[maxCNAMElen] = '\0'; // just in case
  RTCPInstance* videoRTCP =
    RTCPInstance::createNew(*env, &rtcpGroupsockVideo,
			      estimatedSessionBandwidthVideo, CNAME,
			      videoSink, NULL /* we're a server */);
  // Note: This starts RTCP running automatically

  // Add these to our 'Darwin injector':
  injector->addStream(videoSink, videoRTCP);
  ////////// END VIDEO //////////

  // Next, specify the destination Darwin Streaming Server:
  if (!injector->setDestination(dssNameOrAddress, remoteStreamName,
				programName, "LIVE555 Streaming Media")) {
    *env << "injector->setDestination() failed: "
	 << env->getResultMsg() << "\n";
    exit(1);
  }

  *env << "Play this stream (from the Darwin Streaming Server) using the URL:\n"
       << "\trtsp://" << dssNameOrAddress << "/" << remoteStreamName << "\n";

  // Finally, start the streaming:
  *env << "Beginning streaming...\n";
  play();

  env->taskScheduler().doEventLoop(); // does not return

  return 0; // only to prevent compiler warning
}

void afterPlaying(void* clientData) 
{
	return;
}

void play() 
{
  // Open the input file as a 'byte-stream file source':
  ByteStreamFileSource* fileSource
    = ByteStreamFileSource::createNew(*env, inputFileName);
  if (fileSource == NULL) {
    *env << "Unable to open file \"" << inputFileName
	 << "\" as a byte-stream file source\n";
    exit(1);
  }
  	
  // Finally, start playing each sink.
  *env << "Beginning to read from file...\n";
  Boolean start= videoSink->startPlaying(*fileSource, afterPlaying, videoSink);
  fprintf(stderr,"start succeed %d \n",start);
}
#endif
#include <stdio.h>
#include <stdlib.h>
#include "PusherAPI.hh"
#include <unistd.h>
#include "openfile.hh"
#include <pthread.h>
void *videoThread(void *args)
{
    unsigned char *p0=NULL;
    unsigned size0=0;
    unsigned char *p1=NULL;
    unsigned size1=0;
    struct timeval timeout;

	bool sps=false;
	bool pps=false;
	unsigned char tBuf[3*1024*1024];
	unsigned int aDataLen=0;
	bool i_frame=false;

    while(1){
        timeout.tv_sec=0;
        timeout.tv_usec=39000;
		/*该函数在有些终端设备上的超时是严重不准确的，如果出现视频画面播放缓慢，既可能是网络的原因，也可能是超时严重不准确造成的*/
        select(0,NULL,NULL,NULL,&timeout);

      	while(1){
			readframefromfileandsend1(p1,size1);


			/*67 68 65 放在一起写*/
       		// fprintf(stderr,"Find SPS PPS %d\n",((unsigned char)p1[4]&0x1F));

			if(((unsigned char)p1[4]&0x1F)==7){
				aDataLen=0;
				memcpy(&tBuf[aDataLen],p1,size1);
				aDataLen+=size1;
				continue;
			}
			if(((unsigned char)p1[4]&0x1F)==8){
				memcpy(&tBuf[aDataLen],p1,size1);
				aDataLen+=size1;
				continue;
			}
			if(((unsigned char)p1[4]&0x1F)==6){
				memcpy(&tBuf[aDataLen],p1,size1);
				aDataLen+=size1;
				continue;
			}
			if(((unsigned char)p1[4]&0x1F)==5){
				memcpy(&tBuf[aDataLen],p1,size1);

				aDataLen+=size1;

				/* 传入视频帧结构 */
				Frame tFrame;
				tFrame.buffer=tBuf;
				tFrame.bufferLen=aDataLen;
				tFrame.type=VIDEO_TYPE;
        		pusher_pushFrame(tFrame);
            	i_frame=true;
        	}
			break;
        }
        if(i_frame){
            i_frame=false;
            continue;
        }

        /* 传入视频帧结构 */
        Frame tFrame;
		tFrame.buffer=p1;
		tFrame.bufferLen=size1;
		tFrame.type=VIDEO_TYPE;
		
       	/* s/p帧 */
        pusher_pushFrame(tFrame);
    }
}

/* audio(aac) */
int getADTSframe(unsigned char* buffer, int buf_size, unsigned char* data ,int* data_size){
    int size = 0;

    if(!buffer || !data || !data_size ){
        return -1;
    }

    while(1){
        if(buf_size  < 7 ){
            return -1;
        }
        //Sync words
        if((buffer[0] == 0xff) && ((buffer[1] & 0xf0) == 0xf0) ){
            size |= ((buffer[3] & 0x03) <<11);     //high 2 bit
            size |= buffer[4]<<3;                //middle 8 bit
            size |= ((buffer[5] & 0xe0)>>5);        //low 3bit
            break;
        }
        --buf_size;
        ++buffer;
    }

    if(buf_size < size){
        return 1;
    }

    memcpy(data, buffer, size);
    *data_size = size;

    return 0;
}

int simplest_aac_parser(char *url)
{
    int data_size = 0;
    int size = 0;
    int cnt=0;
    int offset=0;

    //FILE *myout=fopen("output_log.txt","wb+");
    FILE *myout=stdout;

    unsigned char *aacframe=(unsigned char *)malloc(1024*5);
    unsigned char *aacbuffer=(unsigned char *)malloc(20*1024*1024);

    FILE *ifile = fopen(url, "rb");
    if(!ifile){
        fprintf(stderr,"Open file error");
        return -1;
    }

    fprintf(stderr,"-----+- ADTS Frame Table -+------+\n");
    fprintf(stderr," NUM | Profile | Frequency| Size |\n");
    fprintf(stderr,"-----+---------+----------+------+\n");

	int sendFrameNums=0;
    while(!feof(ifile)){
        data_size = fread(aacbuffer+offset, 1, 20*1024*1024-offset, ifile);
        unsigned char* input_data = aacbuffer;

        while(1)
        {
            int ret=getADTSframe(input_data, data_size, aacframe, &size);
            if(ret==-1){
                break;
            }else if(ret==1){
                memcpy(aacbuffer,input_data,data_size);
                offset=data_size;
                break;
            }
			usleep(23219);

			/* 传入音频帧结构 */
			Frame tFrame;
			tFrame.buffer=aacframe;
			tFrame.bufferLen=size;
			tFrame.type=AUDIO_TYPE;
            pusher_pushFrame(tFrame);

            data_size -= size;
            input_data += size;
        }
    }
    
    fclose(ifile);
    free(aacbuffer);
    free(aacframe);

    return 0;
}


void *audioThread(void *arg)
{
	while(1){
		simplest_aac_parser("/root/test.aac");
		break;
	}

}
int main(int argc,char *argv[])
{
	/* 初始化推流库 */
	pusher_init();

	/* 推流库连接到服务器 */
	pusher_connect("192.168.8.111",
						554,
						"test101.sdp",
						"",
						"");
	/* 打开视频文件 */
	openVediofile1();
	usleep(1000*30);
	
	/*创建写数据线程*/
    pthread_t vThread;
    pthread_create(&vThread,NULL,videoThread,NULL);
    pthread_t aThread;
    pthread_create(&aThread,NULL,audioThread,NULL);

	while(1){
		usleep(1000*1000);
	}
	return 0;
}
