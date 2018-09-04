#include <stdio.h>
#include <stdlib.h>
#include "Until.h"
#include "TimeoutJob.h"
#include "TCPSocket.h"
#include "EventJob.h"
#include "TCPListener.h"
#include "getopt.h"
#include "Log.h"
#include "SocketHelper.h"
#include "ClientEventJob.h"
#include "ServerEventJob.h"
/*
	测试超时功能
*/
class Timer1:public TimeoutJob
{
	public:
		Timer1(){}
		~Timer1(){}
		virtual int Processor()
		{
			static int t=0;
			fprintf(stderr,"---------- > Test1 %d\n",++t);
			return 1;
		}
};
/*
	测试超时功能
*/
class Timer2:public TimeoutJob
{
	public:
		Timer2(){}
		~Timer2(){}
		virtual int Processor()
		{
			static int t=0;
			fprintf(stderr,"Test2 %d\n",++t);
			return 1;
		}
};
/*@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@*/
/*
	 测试客户端连接功能
*/
/*
	定义一个超时任务，用于周期性的发送数据
	该任务需要受socket拥有者的控制，防止网络异常，引起任务崩溃
*/
class Timer3:public TimeoutJob 
{
	public:
		Timer3():socket(NULL)
		{
			this->setScheduleTime(1000);
			this->Signal();
		}
		~Timer3(){}
		void setSocket(TCPSocket *s){socket=s;}
		virtual int Processor()
		{
			if(getKillSelftEvent()){
				fprintf(stderr,"Kill TimeoutJob\n");
				return 0;
			}
			char *buf="hello server";
			unsigned int sendLen;
			int theErr=socket->send(buf,strlen(buf),&sendLen);
			#if 0/*该任务不负责检测网络异常*/
			if(!socket->isConnected()){
				fprintf(stderr,"2. errno %d theErr %d\n",errno,theErr);
				return 0;
			}
			#endif
			fprintf_debug("%s %d\n",buf,sendLen);	
			return 1;
		}
		private:
			TCPSocket *socket;
};
class ClientJob:public ClientEventJob /*定义一个客户端任务*/
{
	public:
		ClientJob(){}
		~ClientJob(){}
		void setT(Timer3 *t){tt5=t;}
		virtual int Processor()
		{
			char buf[128]={0};
			unsigned int bufLen=128;
			unsigned int outLen=0;
			((TCPSocket *)getSocket())->read(buf,bufLen,&outLen);
			
			if(!(((TCPSocket *)getSocket())->isConnected())){
				tt5->setkillSelfEvent(); /*网络已断，通知Test5任务，不要发送数据并退出*/
				fprintf(stderr,"2. errno %d \n",errno);
				return 0;
			}
			return 1;
		}
		private:
			Timer3 *tt5;
};
/*@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@*/

/*====================================================== 服务器 ================================*/
/*
	测试服务端监听功能
*/
class TestTCPClientSession:public ServerEventJob /*定义客户端任务*/
{
	public:
		TestTCPClientSession(){}
		~TestTCPClientSession(){}
		virtual int Processor()
		{
			char buf[1024]={0};
			unsigned int recvLen;
			int flags=((TCPSocket *)getSocket())->read(buf,sizeof(buf),&recvLen);
			int state=((TCPSocket *)getSocket())->isConnected();
			fprintf(stderr,"======= %d\n",state);
			if(!state){
				fprintf(stderr,"Server Exit\n");
				return 0;
			}
			fprintf_msg("%s recvLen %d\n",buf,recvLen);
			return 1;
		}
		
};
/*定义监听器*/
class Listener:public TCPListener  
{
	public:
		Listener(){}
		virtual ~Listener(){}
		virtual EventJob *GetSessionJob(TCPSocket** outSocket)
		{
			EventJob *theJob=new TestTCPClientSession(); 
			*outSocket=(TCPSocket *)theJob->getSocket();
			return theJob;
		}
};
/*============================================================================================*/
int main(int argc,char *argv[])
{
	/*初始化系统环境，必须第一个调用*/
	InitializeENV();
	/*初始化网络环境*/
	SocketHelper::Initialize();

    int ch;
	bool createClient=false;
	bool createServer=false;
#if defined(__WIN32__) || defined(_WIN32) || defined(_WIN32_WCE)
	if(strcmp(argv[1],"-c")==0){
		createClient=true;
	}
	if(!strcmp(argv[1],"-s")==0){
		createServer=true;
	}
#else
	while ((ch = ::getopt(argc,argv, "cs")) != EOF) /*暂时只测试 client server*/
	{
		switch(ch){
			case 'c':{
				createClient=true;
				break;
			}
			case 's':{
				createServer=true;
				break;
			}
		}
	}

#endif
#if 1
		/*启动服务端*/
	Listener*l1=NULL;
	Listener*l2=NULL;
	if(createServer){
		if(argc<4){
			fprintf(stderr,"Do not Create Server \n");
			exit(1);
		}
		fprintf(stderr,"Create Server\n");
		unsigned short serverPort=::atoi(argv[3]);
		l1=new Listener; /*创建监听器实例*/
		l1->Initialize(l1->getSocket()->strIpToUInt32(argv[2]),serverPort);
		l1->Signal(SOCKET_READABLE);

		l2=new Listener; /*创建监听器实例*/
		l2->Initialize(l2->getSocket()->strIpToUInt32(argv[2]),60010);
		l2->Signal(SOCKET_READABLE);
		fprintf_msg("Create Server\n");
	}
#endif
#if 1	

	/*启动客户端*/
	ClientJob *cj1=NULL;
	Timer3 *ct1=NULL;
	if(createClient){
		if(argc<3){
			fprintf(stderr,"Do not Create Client \n");
			exit(1);
		}
		unsigned int port=::atoi(argv[3]);
		cj1=new ClientJob; /*创建客户端任务*/
		int flags=cj1->Initialize(cj1->getSocket()->strIpToUInt32(argv[2]),port); /*初始化，链接服务器*/
		fprintf(stderr,"flags %d \n",flags);
		if(flags!=0){
			cj1->Signal(SOCKET_READABLE); /**/
		
			ct1=new Timer3(); /*创建超时任务*/
			ct1->setScheduleTime(1000); /*1秒发送一次数据*/
			ct1->setSocket((TCPSocket *)cj1->getSocket()); /*获取 ClientJob 的socket*/

			cj1->setT(ct1);
			
			fprintf_msg("Create Client\n");			
		}
		else{delete ct1;}
		
	}
#endif	

	
	/*超时任务测试*/
	#if 1
	Timer1 t1;
	Timer2 t2;
	t1.setScheduleTime(1000);
	t2.setScheduleTime(5000);

	t1.Signal();
	//t2.Signal();

	#endif
	Job::Run();

	fprintf(stderr,"%s %d \n",__FUNCTION__,__LINE__);
	//	fprintf(stderr,"hello world\n");
	//	fprintf(stderr,"local Ip %s\n",  SocketHelper::GetIPAddrStr(0)->GetAsCString());
	//	fprintf(stderr,"local Ip %s\n",  SocketHelper::GetIPAddrStr(1)->GetAsCString());
	//	SleepBymSec(100);
	return 1;
}
