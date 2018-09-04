#include "OSQueue.h"


OSQueue::OSQueue() : fLength(0)
{
    fSentinel.fNext = &fSentinel;
    fSentinel.fPrev = &fSentinel;
}

void OSQueue::EnQueue(OSQueueElem* elem)
{
    Assert(elem != NULL);
    if (elem->fQueue == this)
        return;
    Assert(elem->fQueue == NULL);
    elem->fNext = fSentinel.fNext;
    elem->fPrev = &fSentinel;
    elem->fQueue = this;
    fSentinel.fNext->fPrev = elem;
    fSentinel.fNext = elem;
    fLength++;
}

OSQueueElem* OSQueue::DeQueue()
{
    if (fLength > 0)
    {
        OSQueueElem* elem = fSentinel.fPrev;
        Assert(fSentinel.fPrev != &fSentinel);
        elem->fPrev->fNext = &fSentinel;
        fSentinel.fPrev = elem->fPrev;
        elem->fQueue = NULL;
        fLength--;
        return elem;
    }
    else
        return NULL;
}

void OSQueue::Remove(OSQueueElem* elem)
{
    Assert(elem != NULL);
    Assert(elem != &fSentinel);
    
    if (elem->fQueue == this)
    {
        elem->fNext->fPrev = elem->fPrev;
        elem->fPrev->fNext = elem->fNext;
        elem->fQueue = NULL;
        fLength--;
    }
}

#if OSQUEUETESTING
Bool16 OSQueue::Test()
{
    OSQueue theVictim;
    void *x = (void*)1;
    OSQueueElem theElem1(x);
    x = (void*)2;
    OSQueueElem theElem2(x);
    x = (void*)3;
    OSQueueElem theElem3(x);
    
    if (theVictim.GetHead() != NULL)
        return false;
    if (theVictim.GetTail() != NULL)
        return false;
    
    theVictim.EnQueue(&theElem1);
    if (theVictim.GetHead() != &theElem1)
        return false;
    if (theVictim.GetTail() != &theElem1)
        return false;
    
    OSQueueElem* theElem = theVictim.DeQueue();
    if (theElem != &theElem1)
        return false;
    
    if (theVictim.GetHead() != NULL)
        return false;
    if (theVictim.GetTail() != NULL)
        return false;
    
    theVictim.EnQueue(&theElem1);
    theVictim.EnQueue(&theElem2);

    if (theVictim.GetHead() != &theElem1)
        return false;
    if (theVictim.GetTail() != &theElem2)
        return false;
        
    theElem = theVictim.DeQueue();
    if (theElem != &theElem1)
        return false;

    if (theVictim.GetHead() != &theElem2)
        return false;
    if (theVictim.GetTail() != &theElem2)
        return false;

    theElem = theVictim.DeQueue();
    if (theElem != &theElem2)
        return false;

    theVictim.EnQueue(&theElem1);
    theVictim.EnQueue(&theElem2);
    theVictim.EnQueue(&theElem3);

    if (theVictim.GetHead() != &theElem1)
        return false;
    if (theVictim.GetTail() != &theElem3)
        return false;

    theElem = theVictim.DeQueue();
    if (theElem != &theElem1)
        return false;

    if (theVictim.GetHead() != &theElem2)
        return false;
    if (theVictim.GetTail() != &theElem3)
        return false;

    theElem = theVictim.DeQueue();
    if (theElem != &theElem2)
        return false;

    if (theVictim.GetHead() != &theElem3)
        return false;
    if (theVictim.GetTail() != &theElem3)
        return false;

    theElem = theVictim.DeQueue();
    if (theElem != &theElem3)
        return false;

    theVictim.EnQueue(&theElem1);
    theVictim.EnQueue(&theElem2);
    theVictim.EnQueue(&theElem3);
    
    OSQueueIter theIterVictim(&theVictim);
    if (theIterVictim.IsDone())
        return false;
    if (theIterVictim.GetCurrent() != &theElem3)
        return false;
    theIterVictim.Next();
    if (theIterVictim.IsDone())
        return false;
    if (theIterVictim.GetCurrent() != &theElem2)
        return false;
    theIterVictim.Next();
    if (theIterVictim.IsDone())
        return false;
    if (theIterVictim.GetCurrent() != &theElem1)
        return false;
    theIterVictim.Next();
    if (!theIterVictim.IsDone())
        return false;
    if (theIterVictim.GetCurrent() != NULL)
        return false;

    theVictim.Remove(&theElem1);

    if (theVictim.GetHead() != &theElem2)
        return false;
    if (theVictim.GetTail() != &theElem3)
        return false;

    theVictim.Remove(&theElem1);

    if (theVictim.GetHead() != &theElem2)
        return false;
    if (theVictim.GetTail() != &theElem3)
        return false;

    theVictim.Remove(&theElem3);

    if (theVictim.GetHead() != &theElem2)
        return false;
    if (theVictim.GetTail() != &theElem2)
        return false;

    return true;
}   
#endif



void OSQueueIter::Next()
{
    if (fCurrentElemP == fQueueP->GetTail())
        fCurrentElemP = NULL;
    else
        fCurrentElemP = fCurrentElemP->Prev();
}


OSQueueElem* OSQueue_Blocking::DeQueueBlocking(OSThread* inCurThread, SInt32 inTimeoutInMilSecs)
{
    OSMutexLocker theLocker(&fMutex);
#ifdef __Win32_
     if (fQueue.GetLength() == 0) 
	 {	fCond.Wait(&fMutex, inTimeoutInMilSecs);
		return NULL;
	 }
#else
    if (fQueue.GetLength() == 0) 
        fCond.Wait(&fMutex, inTimeoutInMilSecs);
#endif

    OSQueueElem* retval = fQueue.DeQueue();
    return retval;
}

OSQueueElem*    OSQueue_Blocking::DeQueue()
{
    OSMutexLocker theLocker(&fMutex);
    OSQueueElem* retval = fQueue.DeQueue(); 
    return retval;
}


void OSQueue_Blocking::EnQueue(OSQueueElem* obj)
{
    {
        OSMutexLocker theLocker(&fMutex);
        fQueue.EnQueue(obj);
    }
    fCond.Signal();
}
