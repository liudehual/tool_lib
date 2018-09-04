#ifndef __STRINGFORMATTER_H__
#define __STRINGFORMATTER_H__

#include <string.h>
#include "StrPtrLen.h"
#include "MyAssert.h"


//Use a class like the ResizeableStringFormatter if you want a buffer that will dynamically grow
class StringFormatter
{
    public:
        
        //pass in a buffer and length for writing
        StringFormatter(char *buffer, UInt32 length) :  fCurrentPut(buffer), 
                                                        fStartPut(buffer),
                                                        fEndPut(buffer + length),
                                                        fBytesWritten(0) {}

        StringFormatter(StrPtrLen &buffer) :            fCurrentPut(buffer.Ptr),
                                                        fStartPut(buffer.Ptr),
                                                        fEndPut(buffer.Ptr + buffer.Len),
                                                        fBytesWritten(0) {}
        virtual ~StringFormatter() {}
        
        void Set(char *buffer, UInt32 length)   {   fCurrentPut = buffer; 
                                                    fStartPut = buffer;
                                                    fEndPut = buffer + length;
                                                    fBytesWritten= 0;
                                                }
                                            
        //"erases" all data in the output stream save this number
        void        Reset(UInt32 inNumBytesToLeave = 0)
            { fCurrentPut = fStartPut + inNumBytesToLeave; }

        //Object does no bounds checking on the buffer. That is your responsibility!
        //Put truncates to the buffer size
        void        Put(const SInt32 num);
        void        Put(char* buffer, UInt32 bufferSize);
        void        Put(char* str)      { Put(str, strlen(str)); }
        void        Put(const StrPtrLen &str) { Put(str.Ptr, str.Len); }
        void        PutSpace()          { PutChar(' '); }
        void        PutEOL()            {  Put(sEOL, sEOLLen); }
        void        PutChar(char c)     { Put(&c, 1); }
        void        PutTerminator()     { PutChar('\0'); }
            
        inline UInt32       GetCurrentOffset();
        inline UInt32       GetSpaceLeft();
        inline UInt32       GetTotalBufferSize();
        char*               GetCurrentPtr()     { return fCurrentPut; }
        char*               GetBufPtr()         { return fStartPut; }
        
        // Counts total bytes that have been written to this buffer (increments
        // even when the buffer gets reset)
        void                ResetBytesWritten() { fBytesWritten = 0; }
        UInt32              GetBytesWritten()   { return fBytesWritten; }
        
        inline void         PutFilePath(StrPtrLen *inPath, StrPtrLen *inFileName);
        inline void         PutFilePath(char *inPath, char *inFileName);

    protected:

        //If you fill up the StringFormatter buffer, this function will get called. By
        //default, no action is taken. But derived objects can clear out the data and reset the buffer
        //Use the ResizeableStringFormatter if you want a buffer that will dynamically grow
        virtual void    BufferIsFull(char* /*inBuffer*/, UInt32 /*inBufferLen*/) { }

        char*       fCurrentPut;
        char*       fStartPut;
        char*       fEndPut;
        
        // A way of keeping count of how many bytes have been written total
        UInt32 fBytesWritten;

        static char*    sEOL;
        static UInt32   sEOLLen;
};

inline UInt32 StringFormatter::GetCurrentOffset()
{
    Assert(fCurrentPut >= fStartPut);
    return (UInt32)(fCurrentPut - fStartPut);
}

inline UInt32 StringFormatter::GetSpaceLeft()
{
    Assert(fEndPut >= fCurrentPut);
    return (UInt32)(fEndPut - fCurrentPut);
}

inline UInt32 StringFormatter::GetTotalBufferSize()
{
    Assert(fEndPut >= fStartPut);
    return (UInt32)(fEndPut - fStartPut);
}

inline void StringFormatter::PutFilePath(StrPtrLen *inPath, StrPtrLen *inFileName)
{
   if (inPath != NULL && inPath->Len > 0)
    {   
        Put(inPath->Ptr, inPath->Len);
        if (kPathDelimiterChar != inPath->Ptr[inPath->Len -1] )
            Put(kPathDelimiterString);
    }
    if (inFileName != NULL && inFileName->Len > 0)
        Put(inFileName->Ptr, inFileName->Len);
}

inline void StringFormatter::PutFilePath(char *inPath, char *inFileName)
{
   StrPtrLen pathStr(inPath);
   StrPtrLen fileStr(inFileName);
   
   PutFilePath(&pathStr,&fileStr);
}

#endif // __STRINGFORMATTER_H__

