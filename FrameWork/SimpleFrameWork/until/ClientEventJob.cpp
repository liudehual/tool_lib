#include "ClientEventJob.h"
#include "TCPSocket.h"

ClientEventJob::ClientEventJob():EventJob(new TCPSocket())
{
}
ClientEventJob::~ClientEventJob()
{
}
int ClientEventJob::Initialize(unsigned int addr, unsigned short port)
{
	((TCPSocket *)getSocket())->open();
	/* 此处有Bug
		连接服务器时容易卡死
	*/
	int theErr=((TCPSocket *)getSocket())->connect(addr,port);
	if(theErr!=-1){
		return 1; 
	}
	fprintf(stderr,"Connect is Error errno %d theErr %d\n",errno,theErr);

	return 0; 
}
