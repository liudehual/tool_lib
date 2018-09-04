#ifndef _IDLETASK_H_
#define _IDLETASK_H_

#include "Task.h"

#include "OSThread.h"
#include "OSHeap.h"
#include "OSMutex.h"
#include "OSCond.h"

class IdleTask;

//merely a private implementation detail of IdleTask
class IdleTaskThread : private OSThread
{
private:

    IdleTaskThread() : OSThread(), fHeapMutex() {}
    virtual ~IdleTaskThread() { Assert(fIdleHeap.CurrentHeapSize() == 0); }

    void SetIdleTimer(IdleTask *idleObj, SInt64 msec);
    void CancelTimeout(IdleTask *idleObj);
    
    virtual void Entry();
    OSHeap  fIdleHeap;
    OSMutex fHeapMutex;
    OSCond  fHeapCond;
    friend class IdleTask;
};


class IdleTask : public Task
{

public:

    //Call Initialize before using this class
    static void Initialize();
    
    IdleTask() : Task(), fIdleElem() { this->SetTaskName("IdleTask"); fIdleElem.SetEnclosingObject(this); }
    
    //This object does a "best effort" of making sure a timeout isn't
    //pending for an object being deleted. In other words, if there is
    //a timeout pending, and the destructor is called, things will get cleaned
    //up. But callers must ensure that SetIdleTimer isn't called at the same
    //time as the destructor, or all hell will break loose.
    virtual ~IdleTask();
    
    //SetIdleTimer:
    //This object will receive an OS_IDLE event in the following number of milliseconds.
    //Only one timeout can be outstanding, if there is already a timeout scheduled, this
    //does nothing.
    void SetIdleTimer(SInt64 msec) { sIdleThread->SetIdleTimer(this, msec); }

    //CancelTimeout
    //If there is a pending timeout for this object, this function cancels it.
    //If there is no pending timeout, this function does nothing.
    //Currently not supported because OSHeap doesn't support random remove
    void CancelTimeout() { sIdleThread->CancelTimeout(this); }

private:

    OSHeapElem fIdleElem;

    //there is only one idle thread shared by all idle tasks.
    static IdleTaskThread*  sIdleThread;    

    friend class IdleTaskThread;
};
#endif
