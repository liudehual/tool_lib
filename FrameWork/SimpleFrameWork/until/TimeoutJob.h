#ifndef __TIMEOUT_JOB_H__
#define __TIMEOUT_JOB_H__
#include "Job.h"
/*
	ʱ�侫ȷ�� 10ms
		
*/
class TimeoutJob:public Job
{
	protected:
		TimeoutJob(unsigned long long mSec=1000); /*���û��ָ����Ĭ������һ��*/
	public:
		virtual ~TimeoutJob(){}


		virtual int Signal(unsigned int conditionSet=0);
		
		void setScheduleTime(unsigned long long mSec); /*�޸ĳ�ʱ*/
		unsigned long long getScheduleTime(); /*��ȡ��ʱ*/

		void addCurrentScheduleTime(unsigned long long t); /*���ӵ�ǰ�ѹ�ʱ��*/
		unsigned long long getCurrentScheduleTime();   /*��ȡ��ǰ�ѹ�ʱ��*/
		void refreshCurrentScheduleTime();	 /*�ص�ǰ�ѹ�ʱ��*/
	private:
		unsigned long long timeout; /*��ʱ*/

		unsigned long long currentTimeout;/*��ǰ�ѹ�ʱ��*/
};

#endif

