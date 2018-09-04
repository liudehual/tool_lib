#ifndef _OSQUEUE_H_
#define _OSQUEUE_H_

#include "MyAssert.h"
#include "OSHeaders.h"
#include "OSMutex.h"
#include "OSCond.h"
#include "OSThread.h"

#define OSQUEUETESTING 0

class OSQueue;

class OSQueueElem {
    public:
        OSQueueElem(void* enclosingObject = NULL) : fNext(NULL), fPrev(NULL), fQueue(NULL),
                                                    fEnclosingObject(enclosingObject) {}
        virtual ~OSQueueElem() { Assert(fQueue == NULL); }

        Bool16 IsMember(const OSQueue& queue) { return (&queue == fQueue); }
        Bool16 IsMemberOfAnyQueue()     { return fQueue != NULL; }
        void* GetEnclosingObject()  { return fEnclosingObject; }
        void SetEnclosingObject(void* obj) { fEnclosingObject = obj; }

        OSQueueElem* Next() { return fNext; }
        OSQueueElem* Prev() { return fPrev; }
        OSQueue* InQueue()  { return fQueue; }
        inline void Remove();

    private:

        OSQueueElem*    fNext;
        OSQueueElem*    fPrev;
        OSQueue *       fQueue;
        void*           fEnclosingObject;

        friend class    OSQueue;
};

class OSQueue {
    public:
        OSQueue();
        ~OSQueue() {}

        void            EnQueue(OSQueueElem* object);
        OSQueueElem*    DeQueue();

        OSQueueElem*    GetHead() { if (fLength > 0) return fSentinel.fPrev; return NULL; }
        OSQueueElem*    GetTail() { if (fLength > 0) return fSentinel.fNext; return NULL; }
        UInt32          GetLength() { return fLength; }
        
        void            Remove(OSQueueElem* object);

#if OSQUEUETESTING
        static Bool16       Test();
#endif

    protected:

        OSQueueElem     fSentinel;
        UInt32          fLength;
};

class OSQueueIter
{
    public:
        OSQueueIter(OSQueue* inQueue) : fQueueP(inQueue), fCurrentElemP(inQueue->GetHead()) {}
        OSQueueIter(OSQueue* inQueue, OSQueueElem* startElemP ) : fQueueP(inQueue)
            {
                if ( startElemP )
                {   Assert( startElemP->IsMember(*inQueue ) );
                    fCurrentElemP = startElemP;
                
                }
                else
                    fCurrentElemP = NULL;
            }
        ~OSQueueIter() {}
        
        void            Reset() { fCurrentElemP = fQueueP->GetHead(); }
        
        OSQueueElem*    GetCurrent() { return fCurrentElemP; }
        void            Next();
        
        Bool16          IsDone() { return fCurrentElemP == NULL; }
        
    private:
    
        OSQueue*        fQueueP;
        OSQueueElem*    fCurrentElemP;
};

class OSQueue_Blocking
{
    public:
        OSQueue_Blocking() {}
        ~OSQueue_Blocking() {}
        
        OSQueueElem*    DeQueueBlocking(OSThread* inCurThread, SInt32 inTimeoutInMilSecs);
        OSQueueElem*    DeQueue();//will not block
        void            EnQueue(OSQueueElem* obj);
        
        OSCond*         GetCond()   { return &fCond; }
        OSQueue*        GetQueue()  { return &fQueue; }
		UInt32          GetLength() { return fQueue.GetLength(); }
		
    private:

        OSCond              fCond;
        OSMutex             fMutex;
        OSQueue             fQueue;
};


void    OSQueueElem::Remove()
{
    if (fQueue != NULL)
        fQueue->Remove(this);
}
#endif //_OSQUEUE_H_
