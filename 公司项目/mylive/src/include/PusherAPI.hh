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

	/* 可扩展音频参数 */

	/* 可扩展视频参数 */
};


#if defined(__cplusplus) || defined(c_plusplus)
extern "C" 
{
#endif

	/* 
		函数名: pusher_init
		描述: 初始化pusher库
		参数: 无
		返回值: 1 成功 0 失败

	*/
	int pusher_init();

	/* 
		函数名: pusher_connect
		描述: 链接到服务器
		参数: 
			serverAddr 服务器ip地址
			serverPort 服务器监听端口
			remoteSDPFileName 指定SDP文件名
			userName   用户名
			userPassword 用户密码
			
		返回值: 1 成功 0 失败

	*/	
	int pusher_connect(char *serverAddr,
						unsigned short serverPort,
						char *remoteSDPFileName,
						char *userName,
						char *userPassword);
						
	/* 
		函数名: pusher_pushFrame
		描述: 压入多媒体帧
		参数:  tFrame 帧描述结构
		返回值: 1 成功

	*/
	int pusher_pushFrame(Frame tFrame);
	
	/* 
		函数名: pusher_deinit
		描述: 反初始化(释放系统资源)
		参数: 无
		返回值: 1 成功

	*/
	int pusher_deinit();
	
	/* 
		函数名: getAudioSleepTime
		描述: 获取音频发送间隔时间
		参数: fSamplingFrequency 音频采样率
		返回值: 发送时间间隔
	*/
	int getAudioSleepTime(int fSamplingFrequency);


#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
