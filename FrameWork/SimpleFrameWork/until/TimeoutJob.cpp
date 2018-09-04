#include "TimeoutJob.h"
#include "Engine.h"
TimeoutJob::TimeoutJob(unsigned long long mSec)
{
	timeout=mSec*1000; //ºÁÃë×ª»»³ÉÎ¢Ãî
	currentTimeout=0;
}
void TimeoutJob::setScheduleTime(unsigned long long mSec)
{
	timeout=mSec*1000;
}
unsigned long long TimeoutJob::getScheduleTime()
{
	return timeout;
}
void TimeoutJob::addCurrentScheduleTime(unsigned long long t)
{
	currentTimeout+=t;
}
unsigned long long TimeoutJob::getCurrentScheduleTime()
{
	return currentTimeout;
}
void TimeoutJob::refreshCurrentScheduleTime()
{
	currentTimeout=0;
}
int TimeoutJob::Signal(unsigned int conditionSet)
{
	Engine *eng=getEngine();
	eng->addElemToTimeoutQueue((void *)this);
	return 1;
}
