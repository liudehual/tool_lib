#ifndef _OSMUTEX_H_
#define _OSMUTEX_H_
#include "PlatformHeader.h"
#include <stdlib.h>
#include "SafeStdLib.h"
#ifndef __Win32__
#include <sys/errno.h>
    #if __PTHREADS_MUTEXES__
        #if __MacOSX__
            #ifndef _POSIX_PTHREAD_H
                #include <pthread.h>
            #endif
        #else
            #include <pthread.h>
        #endif
        #include <unistd.h>
        
    #else
        #include "mymutex.h"
    #endif
#endif

#include "OSHeaders.h"
#include "OSThread.h"
#include "MyAssert.h"

class OSCond;

class OSMutex
{
    public:

        OSMutex();
        ~OSMutex();

        inline void Lock();
        inline void Unlock();
        
        // Returns true on successful grab of the lock, false on failure
        inline Bool16 TryLock();

    private:

#ifdef __Win32__
        CRITICAL_SECTION fMutex;
        
        DWORD       fHolder;
        UInt32      fHolderCount;
        
#elif !__PTHREADS_MUTEXES__
        mymutex_t fMutex;
#else
        pthread_mutex_t fMutex;
        // These two platforms don't implement pthreads recursive mutexes, so
        // we have to do it manually
        pthread_t   fHolder;
        UInt32      fHolderCount;
#endif

#if __PTHREADS_MUTEXES__ || __Win32__       
        void        RecursiveLock();
        void        RecursiveUnlock();
        Bool16      RecursiveTryLock();
#endif
        friend class OSCond;
};

class   OSMutexLocker
{
    public:

        OSMutexLocker(OSMutex *inMutexP) : fMutex(inMutexP) { if (fMutex != NULL) fMutex->Lock(); }
        ~OSMutexLocker() {  if (fMutex != NULL) fMutex->Unlock(); }
        
        void Lock()         { if (fMutex != NULL) fMutex->Lock(); }
        void Unlock()       { if (fMutex != NULL) fMutex->Unlock(); }
        
    private:

        OSMutex*    fMutex;
};

void OSMutex::Lock()
{
#if __PTHREADS_MUTEXES__ || __Win32__
    this->RecursiveLock();
#else
    mymutex_lock(fMutex);
#endif //!__PTHREADS__
}

void OSMutex::Unlock()
{
#if __PTHREADS_MUTEXES__ || __Win32__
    this->RecursiveUnlock();
#else
    mymutex_unlock(fMutex);
#endif //!__PTHREADS__
}

Bool16 OSMutex::TryLock()
{
#if __PTHREADS_MUTEXES__ || __Win32__
    return this->RecursiveTryLock();
#else
    return (Bool16)mymutex_try_lock(fMutex);
#endif //!__PTHREADS__
}

#endif //_OSMUTEX_H_
