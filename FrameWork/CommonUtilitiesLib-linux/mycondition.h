#ifndef _MYCONDITION_H_
#define _MYCONDITION_H_


#include <mach/mach.h>
#include <pthread.h>

#include "mymutex.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void* mycondition_t;
mycondition_t mycondition_alloc();
void mycondition_free(mycondition_t);

void mycondition_broadcast(mycondition_t);
void mycondition_signal(mycondition_t);
void mycondition_wait(mycondition_t, mymutex_t, int); //timeout as a msec offset from now (0 means no timeout)

#ifdef __cplusplus
}
#endif

#endif
