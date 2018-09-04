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
			����ֵ���µĽ��
				0 Job�ᱻ�Ӷ������Ƴ�����ɾ��
				��0 job �ᱻ�ٴ�ִ��  //�˴������ٴ���չ
		*/
		virtual int Processor()=0;
		static int Run();
		ObjectQueueElem *getJobElem(){return &jobElem;}
		/*ÿһ������������������ǿ��ɱ��*/
		bool getKillSelftEvent(){return kKill;}
		void setkillSelfEvent(){kKill=true;}
	protected:
		ObjectQueueElem jobElem;
	private:
	static	Engine eng;
	bool kKill;
};

#endif
