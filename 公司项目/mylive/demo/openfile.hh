#ifndef _RTSP_DEMO_H
#define _RTSP_DEMO_H
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>


#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>

int file_len;
void *pfile,*start_fp;

//����Ƿ���һ֡����
int chece_one_frame(unsigned char * buf){
  if(buf[0]==0 && buf[1]==0 && buf[2]==0 && buf[3]==1 
  	&& (   (buf[4]&0x1F) == 1
  		|| (buf[4]&0x1F) == 5
  		|| (buf[4]&0x1F) == 7
  		|| (buf[4]&0x1F) == 8 
  		|| (buf[4]&0x1F) == 6 )){
      return 1;
    }
  return 0;
}

//��ȡh264 nalu
int get_nalu(unsigned char * buf,unsigned len){
  int i;
  for(i=5;i<len-5;i++){
      if(chece_one_frame(&buf[i])==1){
          return i;
        }
    }
  return 0;
}

//���ļ��ж�ȡ֡����
char * readframefromfileandsend(unsigned char *&buf,unsigned &off)
{

  int ifEnd=file_len-((long)pfile - (long)start_fp); //�ļ�ʣ����
  off = get_nalu((unsigned char *)pfile,ifEnd);
  if(off > 0){
      buf=(unsigned char *)pfile;
      pfile += off;
    }
  if(off<=0){
      pfile = start_fp;
    }
  return NULL;
}

//����Ƶ�ļ�
void openVediofile()
{
  int a = 10;
  int i;
  int fd=-1;

  if(fd < 0){

      if((fd = open("cif.h264", O_RDWR, S_IRWXU)) < 0)
	{ //����Ƶ�ļ�
          printf("open file wrong!");
          exit(1);
        }
      struct stat file_stat;
      if (fstat(fd, &file_stat) < 0 ){ //��ȡ�ļ�����
          printf(" fstat wrong");
          exit(1);
        }
      file_len = file_stat.st_size;
      if(NULL==start_fp){
          if( ( start_fp = mmap(NULL, file_stat.st_size, PROT_READ, MAP_SHARED, fd, 0 )) == MAP_FAILED)
	    { //ӳ���ļ��ڴ�ռ�
              printf("mmap wrong");
              exit(0);
            }
        }
    }
  pfile = start_fp;


}

int file_len1;
void *pfile1,*start_fp1;
//����Ƿ���һ֡����
int chece_one_frame1(unsigned char * buf){
  if(buf[0]==0 && buf[1]==0 && buf[2]==0 && buf[3]==1 
  	&& (   (buf[4]&0x1F) == 1
  		|| (buf[4]&0x1F) == 5
  		|| (buf[4]&0x1F) == 7
  		|| (buf[4]&0x1F) == 8 
  		|| (buf[4]&0x1F) == 6 )){
      return 1;
    }
  return 0;
}
//��ȡh264 nalu
int get_nalu1(unsigned char * buf,unsigned len){
  int i;
  for(i=5;i<len-5;i++){
      if(chece_one_frame1(&buf[i])==1){
          return i;
        }
    }
  return 0;
}
//���ļ��ж�ȡ֡����
char * readframefromfileandsend1(unsigned char *&buf,unsigned &off)
{

  int ifEnd=file_len1-((long)pfile1 - (long)start_fp1); //�ļ�ʣ����
  off = get_nalu1((unsigned char *)pfile1,ifEnd);
  if(off > 0){
      buf=(unsigned char *)pfile1;
      pfile1 += off;
    }
  if(off<=0){
      pfile1 = start_fp1;
    }
  return NULL;
}
//����Ƶ�ļ�
void openVediofile1()
{
  int a = 10;
  int i;
  int fd=-1;

  if(fd < 0){

      if((fd = open("test.h264", O_RDWR, S_IRWXU)) < 0)

	  { //����Ƶ�ļ�
          printf("open file wrong!");
          exit(1);
        }
      fprintf(stderr,"D1.h265 file Describe is %d\n",fd);
      struct stat file_stat;
      if (fstat(fd, &file_stat) < 0 ){ //��ȡ�ļ�����
          printf(" fstat wrong");
          exit(1);
        }
      file_len1 = file_stat.st_size;
      if(NULL==start_fp1){

          if( ( start_fp1 = mmap(NULL, file_stat.st_size, PROT_READ, MAP_SHARED, fd, 0 )) == MAP_FAILED)

		  { //ӳ���ļ��ڴ�ռ�
              printf("mmap wrong");
              exit(0);
            }
        }
    }
  pfile1 = start_fp1;


}


#endif
