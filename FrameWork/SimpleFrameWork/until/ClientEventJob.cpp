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
	/* �˴���Bug
		���ӷ�����ʱ���׿���
	*/
	int theErr=((TCPSocket *)getSocket())->connect(addr,port);
	if(theErr!=-1){
		return 1; 
	}
	fprintf(stderr,"Connect is Error errno %d theErr %d\n",errno,theErr);

	return 0; 
}
