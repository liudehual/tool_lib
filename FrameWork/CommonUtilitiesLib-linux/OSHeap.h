#ifndef _OSHEAP_H_
#define _OSHEAP_H_

#define _OSHEAP_TESTING_ 0

#include "OSCond.h"

class OSHeapElem;

class OSHeap
{
    public:
    
        enum
        {
            kDefaultStartSize = 1024 //UInt32
        };
        
        OSHeap(UInt32 inStartSize = kDefaultStartSize);
        ~OSHeap() { if (fHeap != NULL) delete fHeap; }
        
        //ACCESSORS
        UInt32      CurrentHeapSize() { return fFreeIndex - 1; }
        OSHeapElem* PeekMin() { if (CurrentHeapSize() > 0) return fHeap[1]; return NULL; }
        
        //MODIFIERS
        
        //These are the two primary operations supported by the heap
        //abstract data type. both run in log(n) time.
        void            Insert(OSHeapElem*  inElem);
        OSHeapElem*     ExtractMin() { return Extract(1); }
        //removes specified element from the heap
        OSHeapElem*     Remove(OSHeapElem* elem);
        
#if _OSHEAP_TESTING_
        //returns true if it passed the test, false otherwise
        static Bool16       Test();
#endif
    
    private:
    
        OSHeapElem*     Extract(UInt32 index);
    
#if _OSHEAP_TESTING_
        //verifies that the heap is in fact a heap
        void            SanityCheck(UInt32 root);
#endif
    
        OSHeapElem**    fHeap;
        UInt32          fFreeIndex;
        UInt32          fArraySize;
};

class OSHeapElem
{
    public:
        OSHeapElem(void* enclosingObject = NULL)
            : fValue(0), fEnclosingObject(enclosingObject), fCurrentHeap(NULL) {}
        ~OSHeapElem() {}
        
        //This data structure emphasizes performance over extensibility
        //If it were properly object-oriented, the compare routine would
        //be virtual. However, to avoid the use of v-functions in this data
        //structure, I am assuming that the objects are compared using a 64 bit number.
        //
        void    SetValue(SInt64 newValue) { fValue = newValue; }
        SInt64  GetValue()              { return fValue; }
        void*   GetEnclosingObject()    { return fEnclosingObject; }
		void	SetEnclosingObject(void* obj) { fEnclosingObject = obj; }
        Bool16  IsMemberOfAnyHeap()     { return fCurrentHeap != NULL; }
        
    private:
    
        SInt64  fValue;
        void* fEnclosingObject;
        OSHeap* fCurrentHeap;
        
        friend class OSHeap;
};
#endif //_OSHEAP_H_
