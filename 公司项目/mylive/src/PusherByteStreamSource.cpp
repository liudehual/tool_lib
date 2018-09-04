#include "PusherByteStreamSource.hh"
#include "FramesManager.hh"
#include "PusherThread.hh"
#include "PusherAPI.hh"
PusherByteStreamSource*
PusherByteStreamSource::createNew(UsageEnvironment& env,int frameSize,int mType) {
  PusherByteStreamSource* newSource
    = new PusherByteStreamSource(env,frameSize,mType);
  return newSource;
}

PusherByteStreamSource::PusherByteStreamSource(UsageEnvironment& env,int frameSize,int mType):FramedSource(env)
{
	mediaType=mType;
}

PusherByteStreamSource::~PusherByteStreamSource() 
{
  
}

void PusherByteStreamSource::doGetNextFrame() 
{
 	doReadFromCacheBuffer();
}

void PusherByteStreamSource::doStopGettingFrames() 
{

}

void PusherByteStreamSource::fileReadableHandler(PusherByteStreamSource* source, int /*mask*/) 
{

}

void PusherByteStreamSource::doReadFromCacheBuffer() 
{
	Frame tFrame;
	tFrame.type=mediaType;
	
 	int theErr=Medium::fThread->readData(&tFrame);

 	//fprintf(stderr,"tFrame.type %d \n",tFrame.type);

	if(theErr != 1 || tFrame.bufferLen <= 0 || tFrame.bufferLen > 2*1024*1024){
		nextTask() = envir().taskScheduler().scheduleDelayedTask(1000*10,
				(TaskFunc*)PusherByteStreamSource::readFrameAgain, this);
		return;
	}
	//fprintf(stderr,"doReadFromCacheBuffer bufferLen %d\n",bufferLen);
	
	if(tFrame.bufferLen>fMaxSize){
		memcpy(fTo,tFrame.buffer,fMaxSize);
		fFrameSize=fMaxSize;
	}else{
		memcpy(fTo,tFrame.buffer,tFrame.bufferLen);
		fFrameSize=tFrame.bufferLen;
	}

	// We don't know a specific play time duration for this data,
    // so just record the current time as being the 'presentation time':
    gettimeofday(&fPresentationTime, NULL);
    
	nextTask() = envir().taskScheduler().scheduleDelayedTask(0,
				(TaskFunc*)FramedSource::afterGetting, this);
	return;
}

int PusherByteStreamSource::pushFrame(Frame *tFrame)
{
	manager->writeData(tFrame);
}
void PusherByteStreamSource::readFrameAgain(FramedSource* source) 
{
  	PusherByteStreamSource *pusherSource=(PusherByteStreamSource *)source;
	pusherSource->doGetNextFrame();
}
