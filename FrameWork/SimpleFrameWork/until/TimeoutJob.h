#ifndef __TIMEOUT_JOB_H__
#define __TIMEOUT_JOB_H__
#include "Job.h"
/*
	时间精确度 10ms
		
*/
class TimeoutJob:public Job
{
	protected:
		TimeoutJob(unsigned long long mSec=1000); /*如果没有指定则默认休眠一秒*/
	public:
		virtual ~TimeoutJob(){}


		virtual int Signal(unsigned int conditionSet=0);
		
		void setScheduleTime(unsigned long long mSec); /*修改超时*/
		unsigned long long getScheduleTime(); /*获取超时*/

		void addCurrentScheduleTime(unsigned long long t); /*增加当前已过时间*/
		unsigned long long getCurrentScheduleTime();   /*获取当前已过时间*/
		void refreshCurrentScheduleTime();	 /*重当前已过时间*/
	private:
		unsigned long long timeout; /*超时*/

		unsigned long long currentTimeout;/*当前已过时间*/
};

#endif

