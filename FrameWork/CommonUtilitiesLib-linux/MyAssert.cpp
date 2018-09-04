#include "MyAssert.h"
#include "OSHeaders.h"

static AssertLogger* sLogger = NULL;

void SetAssertLogger(AssertLogger* theLogger)
{
    sLogger = theLogger;
}

void MyAssert(char *inMessage)
{
    if (sLogger != NULL)
        sLogger->LogAssert(inMessage);
    else
    {
	
#if __Win32__
        DebugBreak();
#else
        (*(long*)0) = 0;
#endif
    }
}
