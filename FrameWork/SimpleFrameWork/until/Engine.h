/*

*/
#ifndef __EVENT_LOOP_H__
#define __EVENT_LOOP_H__
#include "Object.h"
#include "NetCommon.h"
#include "ObjectQueue.h"


#define SOCKET_READABLE    (1<<1)
#define SOCKET_WRITABLE    (1<<2)
#define SOCKET_EXCEPTION   (1<<3)

class TimeManager;
class Engine:public Object
{
	public:
		Engine();
		~Engine();
		void start();
		void end();

		int addEvent(void *object,unsigned int conditionSet);
		int delEvent(void *object,unsigned int conditionSet=0);

		int addElemToTimeoutQueue(void *object);
		int delElemFromTimeoutQueue(void *object);
	private:
		void dispatch();
	private:
		fd_set fReadSet;
		fd_set fWriteSet;
		fd_set fExceptionSet;

		int fMaxNumSockets;
		bool runFlags;

		ObjectQueue eventQueue;
		TimeManager *mManager;
};


#endif
