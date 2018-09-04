/*

*/
#ifndef __JOB_H__
#define __JOB_H__

#include "Object.h"
#include "ObjectQueue.h"
#include "Engine.h"
class Job:public Object
{
	protected:
		Job();
		virtual ~Job();
		Engine *getEngine(){return &eng;}
		
	public:
		virtual int Signal(unsigned int conditionSet=0)=0;
		/*
			返回值导致的结果
				0 Job会被从队列中移除，并删除
				非0 job 会被再次执行  //此处可以再次扩展
		*/
		virtual int Processor()=0;
		static int Run();
		ObjectQueueElem *getJobElem(){return &jobElem;}
		/*每一个任务都允许被其他任务强制杀死*/
		bool getKillSelftEvent(){return kKill;}
		void setkillSelfEvent(){kKill=true;}
	protected:
		ObjectQueueElem jobElem;
	private:
	static	Engine eng;
	bool kKill;
};

#endif
