#ifndef __LOG_H__
#define __LOG_H__


/***************************************************************
* ���ļ���Ҫ���� ����ȫ��ע�ͻ���/�ر�ĳһ�������ע��
*	ʹ�ñ�logֻ�������ͷ�ļ�����
****************************************************************/

#define ERR 		
#define WARN 		
#define MSG 		
#define BUG 

#ifdef ERR
#define fprintf_err(fmt,...) \
	do \
		fprintf(stderr,"[LOG_ERR] %s %d "fmt,__FUNCTION__,__LINE__,##__VA_ARGS__);\
	while(0)
#else
#define fprintf_err(fmt,...)
#endif

#ifdef WARN
#define fprintf_warn(fmt,...) \
	do \
		fprintf(stderr,"[LOG_WARN] %s %d "fmt,__FUNCTION__,__LINE__,##__VA_ARGS__);\
	while(0)
#else
#define fprintf_warn(fmt,...)
#endif

#ifdef MSG
#define fprintf_msg(fmt,...) \
	do \
		fprintf(stderr,"[LOG_MSG] "fmt,##__VA_ARGS__);\
	while(0)
#else
#define fprintf_msg(fmt,...)
#endif

#ifdef BUG
#define fprintf_debug(fmt,...) \
	do \
		fprintf(stderr,"[LOG_DEBUG] %s %d "fmt,__FUNCTION__,__LINE__,##__VA_ARGS__);\
	while(0)
#else
#define fprintf_debug(fmt,...)
#endif

#endif
