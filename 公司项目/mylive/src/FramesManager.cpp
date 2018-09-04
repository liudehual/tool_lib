#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "FramesManager.hh"
#include "OSMutex.hh"

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

#define DEFAULT_FRAME_SIZE 100*1024 
#define MAX_FRAME_SIZE 2*1024*1024
FrameContainer::FrameContainer(int frameSize)
					
{
	f=new Frame;
	f->buffer=new unsigned char[frameSize];
	f->bufferLen=0;
	f->type=0x00;
	memoryLen=frameSize;
}
FrameContainer::~FrameContainer()
{

}
void FrameContainer::cleanContainer()
{
	/*just reset contentLen and container's stat*/
	setFreeStat(true);
}

int FrameContainer::writeData(Frame *tFrame)
{
	
	/*we have Max frame size  limit*/
	if(tFrame->bufferLen>memoryLen){
		return 0;
	}
	f->bufferLen=tFrame->bufferLen;
	f->type=tFrame->type;
	//fprintf(stderr,"----> f->type %d \n",f->type);
	memcpy(f->buffer,tFrame->buffer,tFrame->bufferLen);
}
int FrameContainer::readData(Frame *tFrame)
{
		tFrame->buffer=f->buffer;
		tFrame->bufferLen=f->bufferLen;
		tFrame->type=f->type;
		#if 0
		fprintf(stderr,"FrameContainer::readData  "
							"tFrame->buffer %p "
							"tFrame->bufferLen %d "
							"tFrame->type %d \n",
							tFrame->buffer,
							tFrame->bufferLen,
							tFrame->type);
		#endif
		f->bufferLen=0;
		f->type=0;
		return 1;
}

CacheManager::CacheManager(int frameSize,int queueLen):readQueue(NULL),readQueueLen(queueLen),
									writeQueue(NULL),writeQueueLen(queueLen),firstRead(true),
									mutex(new OSMutex)
{
	readQueue=new FrameContainer*[queueLen];
	readQueueLen=queueLen;
	for(int i=0;i<readQueueLen;++i){
		readQueue[i]=new FrameContainer(frameSize);
	}
	writeQueue=new FrameContainer*[queueLen];
	writeQueueLen=queueLen;
	for(int i=0;i<writeQueueLen;++i){
		writeQueue[i]=new FrameContainer(frameSize);
	}
}
CacheManager::~CacheManager()
{
	for(int i=0;i<readQueueLen;++i){
		delete readQueue[i];
		readQueue[i]=NULL;
	}
	delete readQueue;
	for(int i=0;i<writeQueueLen;++i){
		delete writeQueue[i];
		writeQueue[i]=NULL;
	}
	delete writeQueue;
}

void CacheManager::swapQueue()
{

	/*swap readQueue and writeQueue*/
  	mutex->Lock();
  	FrameContainer **tReadQueue=readQueue;
  	readQueue=writeQueue;
  	writeQueue=tReadQueue;

	int tLen=readQueueLen;
  	readQueueLen=writeQueueLen;
	writeQueueLen=tLen;
    mutex->Unlock();
}
int CacheManager::findMediaType()
{
  if(firstRead){ //we
     cleanCache();
    firstRead=false;
  }
  //fprintf(stderr,"%s %d \n",__FUNCTION__,__LINE__);

  /*get one frame*/
  for(int i=0;i<readQueueLen;++i){
		if(!readQueue[i]->getFreeStat()){
			return readQueue[i]->getMediaType();
	//		fprintf(stderr,"%s %d %d\n",__FUNCTION__,__LINE__,bufferLen);
		}
  }
  /*没取到，交换队列*/
  swapQueue();
 // fprintf(stderr,"%s %d \n",__FUNCTION__,__LINE__);

  /*again*/
  for(int i=0;i<readQueueLen;++i){
		if(!readQueue[i]->getFreeStat()){
			return readQueue[i]->getMediaType();
		}
  }
  
  return 0;
}

void CacheManager::cleanCache()
{
	/*clean readQueue and writeQueue*/
	for(int i=0;i<readQueueLen;++i){
		readQueue[i]->cleanContainer();
		readQueue[i]->setFreeStat(true);
	}
	for(int i=0;i<writeQueueLen;++i){
		writeQueue[i]->cleanContainer();
		writeQueue[i]->setFreeStat(true);
	}
}
int CacheManager::writeData(Frame *tFrame)
{
	if(tFrame->bufferLen<=0){
		return 0;
	}
	
	//fprintf(stderr,"%s %d bufferLen %d\n",__FUNCTION__,__LINE__,bufferLen);
	mutex->Lock();
	for(int i=0;i<writeQueueLen;++i){
		FrameContainer *fc=writeQueue[i];
		if(fc->getFreeStat()){
			fc->writeData(tFrame);
			fc->setFreeStat(false);
			mutex->Unlock();	
			return 1;
		}
	}
	#if 1
	for(int i=0;i<writeQueueLen;++i){
		writeQueue[i]->cleanContainer();
		writeQueue[i]->setFreeStat(true);
	}	
	#endif
	mutex->Unlock();
	return 0;
}
int CacheManager::readData(Frame *tFrame)
{
  if(firstRead){ //we
     cleanCache();
    firstRead=false;
  }
  //fprintf(stderr,"%s %d \n",__FUNCTION__,__LINE__);

  /*get one frame*/
  for(int i=0;i<readQueueLen;++i){
		if(!readQueue[i]->getFreeStat()){
			readQueue[i]->readData(tFrame);
	//		fprintf(stderr,"%s %d %d\n",__FUNCTION__,__LINE__,bufferLen);
			readQueue[i]->setFreeStat(true);
			return 1;

		}
  }
  /*没取到，交换队列*/
  swapQueue();
 // fprintf(stderr,"%s %d \n",__FUNCTION__,__LINE__);

  /*again*/
  for(int i=0;i<readQueueLen;++i){
		if(!readQueue[i]->getFreeStat()){
			readQueue[i]->readData(tFrame);
			readQueue[i]->setFreeStat(true);
		}
		return 1;
  }
  
  return 0;
}
