#ifndef __GET_NET_INTERFACE_INFO_H__
#define __GET_NET_INTERFACE_INFO_H__

/*
	��ģ����Ҫ���ڻ�ȡ����������

	������
	ip��ַ
	��������
	�����ַ
*/

typedef struct
{
	char name[16];
	char ip[32];
	char netmask[32];
	char mac[32];
}Info;
/*
	���֧�� 8������ӿ�
*/
#define arrayNums 8

/*
	��ȡ��ǰ����������Ϣ
*/
Info **GetNetInterfaceInfo();

/*
	��ȡָ������IP��ַ���������룬MAC��ַ
	
	����: lpszEth(ָ�������� ��"eth0"��)
	���: szIpAddr
	���: szNetmask
	���: szMacAddr
*/
int DevGetLocalNetInfo(
    const char* lpszEth,
    char* szIpAddr,
    char* szNetmask,
    char* szMacAddr
    );

#endif
