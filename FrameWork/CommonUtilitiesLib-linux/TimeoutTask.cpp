#include "TimeoutTask.h"
#include "OSMemory.h"

TimeoutTaskThread*  TimeoutTask::sThread = NULL;

void TimeoutTask::Initialize()
{
    if (sThread == NULL)
    {
        sThread = NEW TimeoutTaskThread();
        sThread->Signal(Task::kStartEvent);
    }
    
}


TimeoutTask::TimeoutTask(Task* inTask, SInt64 inTimeoutInMilSecs)
: fTask(inTask), fQueueElem()
{
	fQueueElem.SetEnclosingObject(this);
    this->SetTimeout(inTimeoutInMilSecs);
    if (NULL == inTask)
		fTask = (Task *) this;
    Assert(sThread != NULL); // this can happen if RunServer intializes tasks in the wrong order

    OSMutexLocker locker(&sThread->fMutex); 
    sThread->fQueue.EnQueue(&fQueueElem);
}

TimeoutTask::~TimeoutTask()
{
    OSMutexLocker locker(&sThread->fMutex);
    sThread->fQueue.Remove(&fQueueElem);
}

void TimeoutTask::SetTimeout(SInt64 inTimeoutInMilSecs)
{
    fTimeoutInMilSecs = inTimeoutInMilSecs;
    if (inTimeoutInMilSecs == 0)
        fTimeoutAtThisTime = 0;
    else
        fTimeoutAtThisTime = OS::Milliseconds() + fTimeoutInMilSecs;
}

SInt64 TimeoutTaskThread::Run()
{
    //ok, check for timeouts now. Go through the whole queue
    OSMutexLocker locker(&fMutex);
    SInt64 curTime = OS::Milliseconds();
	SInt64 intervalMilli = kIntervalSeconds * 1000;//always default to 60 seconds but adjust to smallest interval > 0
	SInt64 taskInterval = intervalMilli;
	
    for (OSQueueIter iter(&fQueue); !iter.IsDone(); iter.Next())
    {
        TimeoutTask* theTimeoutTask = (TimeoutTask*)iter.GetCurrent()->GetEnclosingObject();
        
        //if it's time to time this task out, signal it
        if ((theTimeoutTask->fTimeoutAtThisTime > 0) && (curTime >= theTimeoutTask->fTimeoutAtThisTime))
        {
#if TIMEOUT_DEBUGGING
            qtss_printf("TimeoutTask %ld timed out. Curtime = %"_64BITARG_"d, timeout time = %"_64BITARG_"d\n",(SInt32)theTimeoutTask, curTime, theTimeoutTask->fTimeoutAtThisTime);
#endif
			theTimeoutTask->fTask->Signal(Task::kTimeoutEvent);
		}
		else
		{
			taskInterval = theTimeoutTask->fTimeoutAtThisTime - curTime;
			if ( (taskInterval > 0) && (theTimeoutTask->fTimeoutInMilSecs > 0) && (intervalMilli > taskInterval) )
				intervalMilli = taskInterval + 1000; // set timeout to 1 second past this task's timeout
#if TIMEOUT_DEBUGGING
			qtss_printf("TimeoutTask %ld not being timed out. Curtime = %"_64BITARG_"d. timeout time = %"_64BITARG_"d\n", (SInt32)theTimeoutTask, curTime, theTimeoutTask->fTimeoutAtThisTime);
#endif
		}
	}
	(void)this->GetEvents();//we must clear the event mask!
	
	OSThread::ThreadYield();
	
#if TIMEOUT_DEBUGGING
	qtss_printf ("TimeoutTaskThread::Run interval seconds= %ld\n", (SInt32) intervalMilli/1000);
#endif
    
    return intervalMilli;//don't delete me!
}
