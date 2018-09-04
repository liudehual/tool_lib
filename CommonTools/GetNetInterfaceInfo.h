#ifndef __GET_NET_INTERFACE_INFO_H__
#define __GET_NET_INTERFACE_INFO_H__

/*
	该模块主要用于获取各个网卡的

	网卡名
	ip地址
	子网掩码
	物理地址
*/

typedef struct
{
	char name[16];
	char ip[32];
	char netmask[32];
	char mac[32];
}Info;
/*
	最多支持 8个网络接口
*/
#define arrayNums 8

/*
	获取当前所有网卡信息
*/
Info **GetNetInterfaceInfo();

/*
	获取指定网卡IP地址，子网掩码，MAC地址
	
	输入: lpszEth(指定网卡名 如"eth0"等)
	输出: szIpAddr
	输出: szNetmask
	输出: szMacAddr
*/
int DevGetLocalNetInfo(
    const char* lpszEth,
    char* szIpAddr,
    char* szNetmask,
    char* szMacAddr
    );

#endif
