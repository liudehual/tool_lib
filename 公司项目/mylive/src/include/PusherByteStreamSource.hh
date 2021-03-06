#ifndef __PUSHER_BITE_STREAM_SOURCE_HH__
#define __PUSHER_BITE_STREAM_SOURCE_HH__
#include "liveMedia.hh"
#include "BasicUsageEnvironment.hh"
#ifndef _FRAMED_FILE_SOURCE_HH
#include "FramedFileSource.hh"
#endif
#include "OSMutex.hh"
class Frame;
class CacheManager;
class PusherByteStreamSource:public FramedSource
{
public:
  static PusherByteStreamSource* createNew(UsageEnvironment& env,int frameSize,int mType);
      // an alternative version of "createNew()" that's used if you already have
      // an open file.

      // 0 means zero-length, unbounded, or unknown

    // if "numBytesToStream" is >0, then we limit the stream to that number of bytes, before treating it as EOF
	int pushFrame(Frame *tFrame);
protected:
  PusherByteStreamSource(UsageEnvironment& env,int frameSize,int mType);
	// called only by createNew()

  virtual ~PusherByteStreamSource();

  static void fileReadableHandler(PusherByteStreamSource* source, int mask);
  void doReadFromCacheBuffer();
  static void readFrameAgain(FramedSource* source);

private:
  // redefined virtual functions:
  virtual void doGetNextFrame();
  virtual void doStopGettingFrames();

private:
	CacheManager *manager;
	OSMutex mutex;
	int mediaType;
	unsigned char buffer[2*1024*1024]; //2M
};
#endif

         
