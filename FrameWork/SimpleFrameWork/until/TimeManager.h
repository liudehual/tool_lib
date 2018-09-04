#ifndef __TIME_MANAGER_H__
#define __TIME_MANAGER_H__
#include "Object.h"
#include "ObjectQueue.h"
class TimeManager:public Object
{
	public:
		TimeManager();
		~TimeManager();

		int addElem(ObjectQueueElem *elem);
		int Processor(); 
		int delElem(void *object);
	private:
		unsigned long long getCurrentTime();
		void synchronizeAndHandle(); 
	private:
		ObjectQueue timeoutQueue; /*ʱ�����*/
		unsigned long long fLastSyncTime; /*���һ��ͬ��ʱ��*/

		bool delJob;

};
#endif

