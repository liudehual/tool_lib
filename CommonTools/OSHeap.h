#ifndef _OSHEAP_H_
#define _OSHEAP_H_

#define _OSHEAP_TESTING_ 0
class OSHeapElem;
class OSHeap
{
    public:
    
        enum
        {
            kDefaultStartSize = 1024 //unsigned int
        };
        
        OSHeap(unsigned int inStartSize = kDefaultStartSize);
        ~OSHeap() { if (fHeap != NULL) delete fHeap; }
        
        //ACCESSORS
        unsigned int      CurrentHeapSize() { return fFreeIndex - 1; }
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
        static unsigned short       Test();
#endif
    
    private:
    
        OSHeapElem*     Extract(unsigned int index);
    
#if _OSHEAP_TESTING_
        //verifies that the heap is in fact a heap
        void            SanityCheck(unsigned int root);
#endif
    
        OSHeapElem**    fHeap;
        unsigned int          fFreeIndex;
        unsigned int          fArraySize;
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
        void    SetValue(signed long long newValue) { fValue = newValue; }
        signed long long  GetValue()              { return fValue; }
        void*   GetEnclosingObject()    { return fEnclosingObject; }
		void	SetEnclosingObject(void* obj) { fEnclosingObject = obj; }
        unsigned short  IsMemberOfAnyHeap()     { return fCurrentHeap != NULL; }
        
    private:
    
        signed long long  fValue;
        void* fEnclosingObject;
        OSHeap* fCurrentHeap;
        
        friend class OSHeap;
};
#endif //_OSHEAP_H_
