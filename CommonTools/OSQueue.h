/*

*/
#ifndef __OS_QUEUE_H__
#define __OS_QUEUE_H__

#include <stdio.h>
#include <stdlib.h>

#define OSQueueTESTING 0

class OSQueue;

class OSQueueElem {
    public:
        OSQueueElem(void* enclosingObject = NULL) : fNext(NULL), fPrev(NULL), fQueue(NULL),
                                                    fEnclosingObject(enclosingObject) {}
        virtual ~OSQueueElem() {}

        unsigned short IsMember(const OSQueue& queue) { return (&queue == fQueue); }
        unsigned short IsMemberOfAnyQueue()     { return fQueue != NULL; }
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
        unsigned int          GetLength() { return fLength; }
        
        void            Remove(OSQueueElem* object);

#if OSQueueTESTING
        static unsigned short       Test();
#endif

    protected:

        OSQueueElem     fSentinel;
        unsigned int          fLength;
};

class OSQueueIter
{
    public:
        OSQueueIter(OSQueue* inQueue) : fQueueP(inQueue), fCurrentElemP(inQueue->GetHead()) {}
        OSQueueIter(OSQueue* inQueue, OSQueueElem* startElemP ) : fQueueP(inQueue)
            {
                if ( startElemP )
                {                   fCurrentElemP = startElemP;
                
                }
                else
                    fCurrentElemP = NULL;
            }
        ~OSQueueIter() {}
        
        void            Reset() { fCurrentElemP = fQueueP->GetHead(); }
        
        OSQueueElem*    GetCurrent() { return fCurrentElemP; }
        void            Next();
        
        unsigned short          IsDone() { return fCurrentElemP == NULL; }
        
    private:
    
        OSQueue*        fQueueP;
        OSQueueElem*    fCurrentElemP;
};
void    OSQueueElem::Remove()
{
    if (fQueue != NULL)
        fQueue->Remove(this);
}
#endif //_OSQueue_H_
