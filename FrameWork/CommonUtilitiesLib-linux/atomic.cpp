#include "atomic.h"
#include "OSMutex.h"

static OSMutex sAtomicMutex;


unsigned int atomic_add(unsigned int *area, int val)
{
    OSMutexLocker locker(&sAtomicMutex);
    *area += val;
    return *area;
}

unsigned int atomic_sub(unsigned int *area,int val)
{
    return atomic_add(area,-val);
}

unsigned int atomic_or(unsigned int *area, unsigned int val)
{
    unsigned int oldval;

    OSMutexLocker locker(&sAtomicMutex);
    oldval=*area;
    *area = oldval | val;
    return oldval;
}

unsigned int compare_and_store(unsigned int oval, unsigned int nval, unsigned int *area)
{
   int rv;
    OSMutexLocker locker(&sAtomicMutex);
    if( oval == *area )
    {
    rv=1;
    *area = nval;
    }
    else
    rv=0;
    return rv;
}
