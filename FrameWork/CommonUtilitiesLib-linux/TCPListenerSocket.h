#ifndef __TCPLISTENERSOCKET_H__
#define __TCPLISTENERSOCKET_H__

#include "TCPSocket.h"
#include "IdleTask.h"

class TCPListenerSocket : public TCPSocket, public IdleTask
{
    public:

        TCPListenerSocket() :   TCPSocket(NULL, Socket::kNonBlockingSocketType), IdleTask(),
                                fAddr(0), fPort(0), fOutOfDescriptors(false), fSleepBetweenAccepts(false) {this->SetTaskName("TCPListenerSocket");}
        virtual ~TCPListenerSocket() {}
        
        //
        // Send a TCPListenerObject a Kill event to delete it.
                
        //addr = listening address. port = listening port. Automatically
        //starts listening
        OS_Error        Initialize(UInt32 addr, UInt16 port);

        //You can query the listener to see if it is failing to accept
        //connections because the OS is out of descriptors.
        Bool16      IsOutOfDescriptors() { return fOutOfDescriptors; }

        void        SlowDown() { fSleepBetweenAccepts = true; }
        void        RunNormal() { fSleepBetweenAccepts = false; }
        //derived object must implement a way of getting tasks & sockets to this object 
        virtual Task*   GetSessionTask(TCPSocket** outSocket) = 0;
        
        virtual SInt64  Run();
            
    private:
    
        enum
        {
            kTimeBetweenAcceptsInMsec = 1000,   //UInt32
            kListenQueueLength = 128            //UInt32
        };

        virtual void ProcessEvent(int eventBits);
        OS_Error    Listen(UInt32 queueLength);

        UInt32          fAddr;
        UInt16          fPort;
        
        Bool16          fOutOfDescriptors;
        Bool16          fSleepBetweenAccepts;
};
#endif // __TCPLISTENERSOCKET_H__

