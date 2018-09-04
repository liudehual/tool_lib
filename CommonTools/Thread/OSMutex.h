/* 
============================================================================================
	��ƽ̨��ʵ��

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

	// ���� ����ͬһ�̼߳���������
	void Lock();

	// ����
	void Unlock();

#if defined(__WIN32__) || defined(_WIN32) || defined(_WIN32_WCE)
	CRITICAL_SECTION * getMutex(){return &m_mutex;}	// �ٽ���
#else
	pthread_mutex_t * getMutex(){return	&m_mutex;}		// ������
#endif
protected:
#if defined(__WIN32__) || defined(_WIN32) || defined(_WIN32_WCE)
	CRITICAL_SECTION	m_mutex;	// �ٽ���
#else
	pthread_mutex_t 	m_mutex;			// ������
#endif
};
#endif
