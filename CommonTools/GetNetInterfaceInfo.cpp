#include "GetNetInterfaceInfo.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>       /* for ifconf */  
#include <linux/sockios.h>    /* for net status mask */  
#include <netinet/in.h>       /* for sockaddr_in */  
#include <sys/socket.h>  
#include <sys/types.h>  
#include <sys/ioctl.h>  
#include <stdio.h>  
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

Info *infoArray[arrayNums]={NULL};
bool initArray=false;

void DevGetNetCardName();
int DevGetLocalNetInfo(const char* lpszEth,int index);

Info **GetNetInterfaceInfo()
{
	if(initArray){
		return infoArray;
	}
	initArray=true;
	DevGetNetCardName();
	for(int i=0;i<arrayNums;++i){
		if(infoArray[i]==NULL){
			continue;
		}
		DevGetLocalNetInfo((const char *)infoArray[i]->name,i);
	}
	return infoArray;
}


void DevGetNetCardName()
{
	  struct ifreq ifr;    
	  struct ifconf ifc;    
	  char buf[2048];    
	  int success = 0;     
	  int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);    
	  if (sock == -1) {
	  	fprintf(stderr,"socket error\n");        
	  	return;    
	  }     
	  ifc.ifc_len = sizeof(buf);    
	  ifc.ifc_buf = buf;    
	  if (::ioctl(sock, SIOCGIFCONF, &ifc) == -1) {        
	  		fprintf(stderr,"ioctl error\n");        
	  		return;    
	  }     
	  struct ifreq* it = ifc.ifc_req;    
	  const struct ifreq* const end = it + (ifc.ifc_len / sizeof(struct ifreq));    
	  char szMac[64];    
	  int count = 0;    
	  int i=0;
	  /* 开始统计网络接口信息 */
	  for (; it != end && i!=arrayNums; ++it) {    
	  		::strcpy(ifr.ifr_name, it->ifr_name);        
	  		if (::ioctl(sock, SIOCGIFFLAGS, &ifr) == 0){            
	  			if ((ifr.ifr_flags)) {               
	  				if (::ioctl(sock, SIOCGIFHWADDR, &ifr) == 0) {                    
	  					count ++ ;                    
	  					unsigned char * ptr ;                    
	  					ptr = (unsigned char  *)&ifr.ifr_ifru.ifru_hwaddr.sa_data[0];                    
	  					snprintf(szMac,64,"%02X:%02X:%02X:%02X:%02X:%02X",*ptr,*(ptr+1),*(ptr+2),*(ptr+3),*(ptr+4),*(ptr+5));                    
	  					fprintf(stderr,"%d,Interface name : %s\n",count,ifr.ifr_name);    
						infoArray[i]=new Info;
						sprintf(infoArray[i]->name,"%s",ifr.ifr_name);
	  					++i;
	  				}            
	  			}        
	  		}else{            
	  			fprintf(stderr,"get mac info error\n");            
	  			return;        
	  		}    
	 	}
		return;
}

// 获取IP地址，子网掩码，MAC地址
int DevGetLocalNetInfo(const char* lpszEth,int index)
{
  if(lpszEth==NULL 
  		|| lpszEth[0]=='\0')
    return 0;
  int ret = 0;

  struct ifreq req;
  struct sockaddr_in* host = NULL;

  int sockfd =::socket(AF_INET, SOCK_DGRAM, 0);
  if ( -1 == sockfd ){
      return -1;
    }
  ::bzero(&req, sizeof(struct ifreq));
  ::strcpy(req.ifr_name, lpszEth);
  if (::ioctl(sockfd, SIOCGIFADDR, &req) >= 0 ){
      host = (struct sockaddr_in*)&req.ifr_addr;
      //strcpy(szIpAddr, inet_ntoa(host->sin_addr));
	  ::strcpy(infoArray[index]->ip,::inet_ntoa(host->sin_addr));
    }else{
      ret = -1;
    }
  ::bzero(&req, sizeof(struct ifreq));
  ::strcpy(req.ifr_name, lpszEth);
  if (::ioctl(sockfd, SIOCGIFNETMASK, &req) >= 0 ){
      host = (struct sockaddr_in*)&req.ifr_addr;
      //strcpy(szNetmask, inet_ntoa(host->sin_addr));
	  ::strcpy(infoArray[index]->netmask,::inet_ntoa(host->sin_addr));
    }else{
      ret = -1;
    }
  ::bzero(&req, sizeof(struct ifreq));
  ::strcpy(req.ifr_name, lpszEth);
  if (::ioctl(sockfd, SIOCGIFHWADDR,&req)>= 0 ){
  #if 0
      sprintf(
            szMacAddr, "%02x:%02x:%02x:%02x:%02x:%02x",
            (unsigned char)req.ifr_hwaddr.sa_data[0],
          (unsigned char)req.ifr_hwaddr.sa_data[1],
          (unsigned char)req.ifr_hwaddr.sa_data[2],
          (unsigned char)req.ifr_hwaddr.sa_data[3],
          (unsigned char)req.ifr_hwaddr.sa_data[4],
          (unsigned char)req.ifr_hwaddr.sa_data[5]
          );
   #endif
   #if 1
	::sprintf(
            infoArray[index]->mac, "%02x:%02x:%02x:%02x:%02x:%02x",
            (unsigned char)req.ifr_hwaddr.sa_data[0],
          	(unsigned char)req.ifr_hwaddr.sa_data[1],
          	(unsigned char)req.ifr_hwaddr.sa_data[2],
          	(unsigned char)req.ifr_hwaddr.sa_data[3],
          	(unsigned char)req.ifr_hwaddr.sa_data[4],
          	(unsigned char)req.ifr_hwaddr.sa_data[5]
          	);
	
   #endif
    }else{
      ret = -1;
    }
  if ( sockfd != -1 ){
      close(sockfd);
      sockfd = -1;
    }
  return ret;
}

// 获取IP地址，子网掩码，MAC地址
int DevGetLocalNetInfo(
    	const char* lpszEth,
    	char* szIpAddr,
    	char* szNetmask,
    	char* szMacAddr	)
{
  if(lpszEth==NULL 
  		|| lpszEth[0]=='\0'
     	|| szIpAddr==NULL 
     	|| szNetmask==NULL 
     	|| szMacAddr==NULL)
    return 0;
    
  int ret = 0;

  struct ifreq req;
  struct sockaddr_in* host = NULL;

  int sockfd = ::socket(AF_INET, SOCK_DGRAM, 0);
  if ( -1 == sockfd ){
     return -1;
  }
  ::bzero(&req, sizeof(struct ifreq));
  ::strcpy(req.ifr_name, lpszEth);
  if (::ioctl(sockfd, SIOCGIFADDR, &req) >= 0 ){
      host = (struct sockaddr_in*)&req.ifr_addr;
      ::strcpy(szIpAddr,::inet_ntoa(host->sin_addr));
  }else{
      ret = -1;
  }
  ::bzero(&req, sizeof(struct ifreq));
  ::strcpy(req.ifr_name, lpszEth);
  if (::ioctl(sockfd, SIOCGIFNETMASK, &req) >= 0 ){
      host = (struct sockaddr_in*)&req.ifr_addr;
      ::strcpy(szNetmask,::inet_ntoa(host->sin_addr));
  }else{
      ret = -1;
  }
  ::bzero(&req, sizeof(struct ifreq));
  ::strcpy(req.ifr_name, lpszEth);
  if (::ioctl(sockfd, SIOCGIFHWADDR,&req)>= 0 ){
      ::sprintf(szMacAddr, "%02x:%02x:%02x:%02x:%02x:%02x",
            	(unsigned char)req.ifr_hwaddr.sa_data[0],
          		(unsigned char)req.ifr_hwaddr.sa_data[1],
          		(unsigned char)req.ifr_hwaddr.sa_data[2],
          		(unsigned char)req.ifr_hwaddr.sa_data[3],
          		(unsigned char)req.ifr_hwaddr.sa_data[4],
          		(unsigned char)req.ifr_hwaddr.sa_data[5]
          	);
    }else{
      ret = -1;
    }
  if (sockfd != -1){
      ::close(sockfd);
      sockfd = -1;
    }
  return ret;
}