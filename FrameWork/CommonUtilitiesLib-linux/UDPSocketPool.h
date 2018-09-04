#ifndef __UDPSOCKETPOOL_H__
#define __UDPSOCKETPOOL_H__

#include "UDPDemuxer.h"
#include "UDPSocket.h"
#include "OSMutex.h"
#include "OSQueue.h"

class UDPSocketPair;

class UDPSocketPool
{
    public:
    
        UDPSocketPool() : fMutex() {}
        virtual ~UDPSocketPool() {}
        
        //Skanky access to member data
        OSMutex*    GetMutex()          { return &fMutex; }
        OSQueue*    GetSocketQueue()    { return &fUDPQueue; }
        
        //Gets a UDP socket out of the pool. 
        //inIPAddr = IP address you'd like this pair to be bound to.
        //inPort = port you'd like this pair to be bound to, or 0 if you don't care
        //inSrcIPAddr = srcIP address of incoming packets for the demuxer.
        //inSrcPort = src port of incoming packets for the demuxer.
        //This may return NULL if no pair is available that meets the criteria.
        UDPSocketPair*  GetUDPSocketPair(UInt32 inIPAddr, UInt16 inPort,
                                            UInt32 inSrcIPAddr, UInt16 inSrcPort);
        
        //When done using a UDP socket pair retrieved via GetUDPSocketPair, you must
        //call this function. Doing so tells the pool which UDP sockets are in use,
        //keeping the number of UDP sockets allocated at a minimum.
        void ReleaseUDPSocketPair(UDPSocketPair* inPair);

        UDPSocketPair*  CreateUDPSocketPair(UInt32 inAddr, UInt16 inPort);
        
    protected:
    
        //Because UDPSocket is a base class, and this pool class is intended to be
        //a general purpose class for all types of UDP sockets (reflector, standard),
        //there must be a virtual fuction for actually constructing the derived UDP sockets
        virtual UDPSocketPair*  ConstructUDPSocketPair() = 0;
        virtual void            DestructUDPSocketPair(UDPSocketPair* inPair) = 0;
        
        virtual void            SetUDPSocketOptions(UDPSocketPair* /*inPair*/) {}
    
    private:
    
        enum
        {
            kLowestUDPPort = 6970,  //UInt16
            kHighestUDPPort = 65535 //UInt16
        };
    
        OSQueue fUDPQueue;
        OSMutex fMutex;
};

class UDPSocketPair
{
    public:
        
        UDPSocketPair(UDPSocket* inSocketA, UDPSocket* inSocketB)
            : fSocketA(inSocketA), fSocketB(inSocketB), fRefCount(0), fElem() {fElem.SetEnclosingObject(this);}
        ~UDPSocketPair() {}
    
        UDPSocket*  GetSocketA() { return fSocketA; }
        UDPSocket*  GetSocketB() { return fSocketB; }
        
    private:
    
        UDPSocket*  fSocketA;
        UDPSocket*  fSocketB;
        UInt32      fRefCount;
        OSQueueElem fElem;
        
        friend class UDPSocketPool;
};
#endif // __UDPSOCKETPOOL_H__

