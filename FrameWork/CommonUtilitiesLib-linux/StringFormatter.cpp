#include <string.h>
#include "StringFormatter.h"
#include "MyAssert.h"

char*   StringFormatter::sEOL = "\r\n";
UInt32  StringFormatter::sEOLLen = 2;

void StringFormatter::Put(const SInt32 num)
{
    char buff[32];
    qtss_sprintf(buff, "%ld", num);
    Put(buff);
}

void StringFormatter::Put(char* buffer, UInt32 bufferSize)
{
    if((bufferSize == 1) && (fCurrentPut != fEndPut)) {
        *(fCurrentPut++) = *buffer;
        fBytesWritten++;
        return;
    }       
        
    //loop until the input buffer size is smaller than the space in the output
    //buffer. Call BufferIsFull at each pass through the loop
    UInt32 spaceLeft = this->GetSpaceLeft();
    UInt32 spaceInBuffer =  spaceLeft - 1;
    UInt32 resizedSpaceLeft = 0;
    
    while ( (spaceInBuffer < bufferSize) || (spaceLeft == 0) ) // too big for destination
    {
        if (spaceLeft > 0)
        {
            ::memcpy(fCurrentPut, buffer, spaceInBuffer);
            fCurrentPut += spaceInBuffer;
            fBytesWritten += spaceInBuffer;
            buffer += spaceInBuffer;
            bufferSize -= spaceInBuffer;
        }
        this->BufferIsFull(fStartPut, this->GetCurrentOffset()); // resize buffer
        resizedSpaceLeft = this->GetSpaceLeft();
        if (spaceLeft == resizedSpaceLeft) // couldn't resize, nothing left to do
        {  
           return; // done. There is either nothing to do or nothing we can do because the BufferIsFull
        }
        spaceLeft = resizedSpaceLeft;
        spaceInBuffer =  spaceLeft - 1;
    }
    
    //copy the remaining chunk into the buffer
    ::memcpy(fCurrentPut, buffer, bufferSize);
    fCurrentPut += bufferSize;
    fBytesWritten += bufferSize;
    
}

