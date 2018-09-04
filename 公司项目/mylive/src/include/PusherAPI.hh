#ifndef __PUSHER_API_HH__
#define __PUSHER_API_HH__

#define DO_NOTHING 0x00
#define VIDEO_TYPE 0x01
#define AUDIO_TYPE 0x02

struct Frame
{
	unsigned char *buffer;
	int bufferLen;
	int type;

	/* ����չ��Ƶ���� */

	/* ����չ��Ƶ���� */
};


#if defined(__cplusplus) || defined(c_plusplus)
extern "C" 
{
#endif

	/* 
		������: pusher_init
		����: ��ʼ��pusher��
		����: ��
		����ֵ: 1 �ɹ� 0 ʧ��

	*/
	int pusher_init();

	/* 
		������: pusher_connect
		����: ���ӵ�������
		����: 
			serverAddr ������ip��ַ
			serverPort �����������˿�
			remoteSDPFileName ָ��SDP�ļ���
			userName   �û���
			userPassword �û�����
			
		����ֵ: 1 �ɹ� 0 ʧ��

	*/	
	int pusher_connect(char *serverAddr,
						unsigned short serverPort,
						char *remoteSDPFileName,
						char *userName,
						char *userPassword);
						
	/* 
		������: pusher_pushFrame
		����: ѹ���ý��֡
		����:  tFrame ֡�����ṹ
		����ֵ: 1 �ɹ�

	*/
	int pusher_pushFrame(Frame tFrame);
	
	/* 
		������: pusher_deinit
		����: ����ʼ��(�ͷ�ϵͳ��Դ)
		����: ��
		����ֵ: 1 �ɹ�

	*/
	int pusher_deinit();
	
	/* 
		������: getAudioSleepTime
		����: ��ȡ��Ƶ���ͼ��ʱ��
		����: fSamplingFrequency ��Ƶ������
		����ֵ: ����ʱ����
	*/
	int getAudioSleepTime(int fSamplingFrequency);


#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
