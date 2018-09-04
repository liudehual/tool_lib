/*
 *
 * @APPLE_LICENSE_HEADER_START@
 * 
 * Copyright (c) 1999-2003 Apple Computer, Inc.  All Rights Reserved.
 * 
 * This file contains Original Code and/or Modifications of Original Code
 * as defined in and that are subject to the Apple Public Source License
 * Version 2.0 (the 'License'). You may not use this file except in
 * compliance with the License. Please obtain a copy of the License at
 * http://www.opensource.apple.com/apsl/ and read it before using this
 * file.
 * 
 * The Original Code and all software distributed under the License are
 * distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT.
 * Please see the License for the specific language governing rights and
 * limitations under the License.
 * 
 * @APPLE_LICENSE_HEADER_END@
 *
 */
/*
    File:       OSCond.h

    Contains:   A simple condition variable abstraction
    
    
        

*/

#ifndef _OSCOND_H_
#define _OSCOND_H_
#if defined(__WIN32__) || defined(_WIN32) || defined(_WIN32_WCE)
	#include <windows.h>
    #include <process.h>
#else
    #include <pthread.h>
#endif

#include "OSMutex.h"
class OSCond 
{
    public:

        OSCond();
        ~OSCond();
        
        inline void     Signal();
        inline void     Wait(OSMutex* inMutex, unsigned int inTimeoutInMilSecs = 0);
        inline void     Broadcast();

    private:

#if defined(__WIN32__) || defined(_WIN32) || defined(_WIN32_WCE)
        HANDLE              fCondition;
        unsigned int              fWaitCount;
#else
        pthread_cond_t      fCondition;
        void                TimedWait(OSMutex* inMutex, unsigned int inTimeoutInMilSecs);
#endif
};

inline void OSCond::Wait(OSMutex* inMutex, unsigned int inTimeoutInMilSecs)
{ 
#if defined(__WIN32__) || defined(_WIN32) || defined(_WIN32_WCE)
    unsigned long theTimeout = INFINITE;
    if (inTimeoutInMilSecs > 0)
        theTimeout = inTimeoutInMilSecs;
    inMutex->Unlock();
    fWaitCount++;
    DWORD theErr = ::WaitForSingleObject(fCondition, theTimeout);
    fWaitCount--;
    inMutex->Lock();
#else
    this->TimedWait(inMutex, inTimeoutInMilSecs);
#endif
}

inline void OSCond::Signal()
{
#if defined(__WIN32__) || defined(_WIN32) || defined(_WIN32_WCE)
    bool theErr = ::SetEvent(fCondition);
#else
    pthread_cond_signal(&fCondition);
#endif
}

inline void OSCond::Broadcast()
{
#if defined(__WIN32__) || defined(_WIN32) || defined(_WIN32_WCE)
    //
    // There doesn't seem like any more elegant way to
    // implement Broadcast using events in Win32.
    // This will work, it may generate spurious wakeups,
    // but condition variables are allowed to generate
    // spurious wakeups
    unsigned int waitCount = fWaitCount;
    for (unsigned int x = 0; x < waitCount; x++)
    {
        BOOL theErr = ::SetEvent(fCondition);
    }
#else
    pthread_cond_broadcast(&fCondition);
#endif
}

#endif //_OSCOND_H_
