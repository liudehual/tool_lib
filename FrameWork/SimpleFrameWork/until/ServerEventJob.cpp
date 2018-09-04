#include "ServerEventJob.h"
#include "TCPSocket.h"
ServerEventJob::ServerEventJob():EventJob(new TCPSocket())
{

}
ServerEventJob::~ServerEventJob()
{

}