/* 
============================================================================================
	跨平台锁实现

	add 2016.09.23 by GCT

	Copyright (c), ...	

=============================================================================================
*/

#ifndef _OSMUTEX_LOCKER_H
#define _OSMUTEX_LOCKER_H

#if defined(__WIN32__) || defined(_WIN32)
#include <windows.h>
#include <process.h>
#else
#include <pthread.h>
#endif
class OSMutex
{
public:
	OSMutex();
	~OSMutex();

	// 加锁 不是同一线程枷锁会阻塞
	void Lock();

	// 解锁
	void Unlock();

#if defined(__WIN32__) || defined(_WIN32) || defined(_WIN32_WCE)
	CRITICAL_SECTION * getMutex(){return &m_mutex;}	// 临界区
#else
	pthread_mutex_t * getMutex(){return	&m_mutex;}		// 互斥锁
#endif
private:
#if defined(__WIN32__) || defined(_WIN32) || defined(_WIN32_WCE)
	CRITICAL_SECTION	m_mutex;	// 临界区
#else
	pthread_mutex_t 	m_mutex;			// 互斥锁
#endif
};
#endif
