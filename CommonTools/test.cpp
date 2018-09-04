#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "base64.h"
#include "daemon.h"
#include "getopt.h"
#include "Log.h"
#include "MakeDir.h"
#include "md5.h"
#include "OSQueue.h"
#include "random.h"
#include "StrPtr.h"
#include "OSQueue.h"
#include "OSHeap.h"
#include "OSThread.h"
#include "OSMutex.h"
#include "OSCond.h"
#include "OSHashTable.h"
#include "GetNetInterfaceInfo.h"
#include <stdio.h>
#include <stdlib.h>

/*
	定义全局条件变量及互斥锁 用于测试
*/

OSCond gCond;
OSMutex gMutex;

/*
	测试线程类 

	
	
*/
class TestThread:public OSThread
{
	public:
	 TestThread(){}
	 ~TestThread(){}
	 virtual void Entry()
	 {
		static int f=0;
		while(1){
			#if 1
			gCond.Wait(&gMutex);
			#endif
		//	getchar();
			fprintf_debug("This is OSThread 0 %d\n",f++);
		}
	}
};
class TestThread1:public OSThread
{
	public:
	 TestThread1(){}
	 ~TestThread1(){}
	 virtual void Entry()
	 {
		static int f=0;
		static int sleepTime=10;
		while(1){
			#if 1
			gCond.Signal();
			OSThread::Sleep(sleepTime);
			fprintf_debug("This is OSThread 1 %d\n",f++);
			sleepTime=100;
			#endif
		}
	}
};

/*测试OSHashTable类*/

/*值*/
class IntValue
{
 public:
	IntValue(){}
	~IntValue(){}
	/*必需有该域*/
	IntValue *fNextHashEntry;
	/*必需有该域*/
	unsigned int          GetHashValue() { return fHashValue; }
   	void SetHashValue(unsigned int value) { fHashValue=value; }
 private:
   	/*必需有该域*/
   	unsigned int fHashValue;
};
	
/*键*/
class IntKey
{
  public:
  	/*构造函数必需如此*/
	IntKey(IntValue *entry)
	{
		/*在此处根据用户自定义规则生成键*/
		SetHashKey(entry->GetHashValue()+1);
	}
	~IntKey(){}
	/*该函数必须被实现*/
	friend int operator ==(const IntKey &key1, const IntKey &key2)
   	{
       	return (key1.fHashValue == key2.fHashValue);
   	}
	/*必需有该域*/
   	unsigned int   GetHashKey() { return fHashValue; }
   	void SetHashKey(unsigned int key) { fHashValue=key; }
  private:
	/*必需有该域*/
   	unsigned int fHashValue;
};
	

int main(int argc,char *argv[])
{

	/* 测试获取网络信息模块 */
#if 0
	Info ** ptr=NULL;
	ptr=GetNetInterfaceInfo();
	for(int i=0;i<arrayNums;++i){
		if(ptr[i]==NULL){
			continue;
		}
		fprintf(stderr,"name %s ip %s mac %s netmask %s\n",
				ptr[i]->name,
				ptr[i]->ip,
				ptr[i]->mac,
				ptr[i]->netmask);
	}
	
	return 1;
#endif

	/*Test Log*/
 #if 0
	fprintf_debug("Debug Start\n");
 #endif
 
	/*Test Random*/ 
 #if 0
	Random *r=new Random;
    fprintf_debug("Debug Random Get Random Number %u\n",r->GetRandom32());

	Random *r1=new Random;
    fprintf_debug("Debug Random Get Random Number %u\n",r1->GetRandom32());
 #endif
 
	/*Test MakeDir */
 #if 0
	RecursiveMakeDir("/home/1/1/1/1/1",0666);
 #endif
 
	char *src="GCT";
	char dst[20]={0};

	/*Test Base64 Encode and Decode*/
 #if 0
	Base64encode(dst,src,strlen(src));
	fprintf_debug("Base64 Encode %s\n",dst);

	char dst1[20]={0};
	Base64decode(dst1,dst);
	fprintf_debug("Base64 Decode %s\n",dst1);
 #endif
 
	/*Test MD5*/
 #if 0
	MD5_CTX md5;
	MD5_Init(&md5);
	char name[]="GCT";
	MD5_Update(&md5,(unsigned char *)name,strlen(name)); //更新数据   
	char dest[128];   
	MD5_Final((unsigned char *)dest,&md5);  //加密结束并将结果放到dest中
	fprintf_debug("MD5 %s\n",dest);
 #endif

	/*Test getopt*/
 #if 0
    int ch;
    unsigned short port=0;
	while((ch = getopt(argc,argv, "vdfxp:DZ:c:o:S:Ih")) != EOF){
		switch(ch)
		{
            case 'v':
				fprintf_debug("Debug Start v \n");

            case'd':
				fprintf_debug("Debug Start d\n");
                
                break;                
            case 'D':
				fprintf_debug("Debug Start D\n");
                   
               break;            
            case 'Z':
				fprintf_debug("Debug Start Z\n");
                                
                break;
            case 'f':
            	fprintf_debug("Debug Start f\n");

                break;
            case 'p':
               	fprintf_debug("Debug Start p\n");
				port=::atoi(optarg);
				fprintf_debug("Port %d\n",port);
                break;
            case 'S':
                fprintf_debug("Debug Start S\n");

                break;
            case 'c':
                fprintf_debug("Debug Start c\n");

                break;
            case 'o':
                fprintf_debug("Debug Start o\n");

                break;
            case 'x':
                fprintf_debug("Debug Start x\n");

                break;
            case 'I':
                fprintf_debug("Debug Start I\n");
                break;
            case 'h':
                fprintf_debug("Debug Start h\n");

            default:
            	fprintf_debug("Option error\n");
                break;
        }
	}
 #endif
	/*Test StrPtr*/
 #if 0
	StrPtr str("hello world");
	fprintf_debug("StrPtr Len %d\n",str.Len);
	StrPtr str1("hello");
	fprintf_debug("find Hello %s\n",str.FindString(str1));
 #endif
 
	/*Test Queue*/
 #if 0
	int a=1;
	int b=2;
	int c=3;

	OSQueueElem	*elem1=new OSQueueElem((void *)&a);
	OSQueueElem	*elem2=new OSQueueElem((void *)&b);
	OSQueueElem	*elem3=new OSQueueElem((void *)&c);

	OSQueue *Queue=new OSQueue;
	Queue->EnQueue(elem1);
	Queue->EnQueue(elem2);
	Queue->EnQueue(elem3);

	for(OSQueueIter it(Queue);!it.IsDone();it.Next()){
		int *tmp=(int *)(it.GetCurrent()->GetEnclosingObject());
		fprintf_debug("Elem Value %d\n",*tmp);
	}
 #endif
 
 #if 0
	/*Test OSHeap*/
	class Test
	{
		public:
			Test()
			{
				fId=(signed long long)r.GetRandom32();
			}
			~Test(){}
			signed long long getId(){return fId;}
		private:
			signed long long fId;
			Random r;
	};

	OSHeap victim(2);
	Test t1;OSHeapElem hElem1(&t1); hElem1.SetValue((signed long long)t1.getId());
	fprintf_debug(" t1 %lld \n",t1.getId());

	Test t2;OSHeapElem hElem2(&t2); hElem2.SetValue((signed long long)t2.getId());
	fprintf_debug(" t2 %lld \n",t2.getId());

	Test t3;OSHeapElem hElem3(&t3); hElem3.SetValue((signed long long)t3.getId());
	fprintf_debug(" t3 %lld \n",t3.getId());

	Test t4;OSHeapElem hElem4(&t4); hElem4.SetValue((signed long long)t4.getId());
	fprintf_debug(" t4 %lld \n",t4.getId());

	Test t5;OSHeapElem hElem5(&t5); hElem5.SetValue((signed long long)t5.getId());
	fprintf_debug(" t5 %lld \n",t5.getId());

    victim.Insert(&hElem1);
    victim.Insert(&hElem2);
    victim.Insert(&hElem3);
    victim.Insert(&hElem4);
    victim.Insert(&hElem5);

    signed long long t=victim.PeekMin()->GetValue();
    Test *rTest=(Test *)(victim.ExtractMin()->GetEnclosingObject());
    fprintf_debug("Min %lld %lld\n",t,rTest->getId());
 #endif

	/*
		Test OSThread class 
		include mutex and cond
	*/
 #if 1
    TestThread tThread;
    tThread.Start();
	TestThread1 tThread1;
    tThread1.Start();
    TestThread tThread2;
    tThread2.Start();
	TestThread1 tThread3;
    tThread3.Start();
 #endif

	/*Test OSHashTable class*/
 #if 0
	typedef class OSHashTable<IntValue,IntKey> OSHashTableMap;
	OSHashTableMap IntMap(100);
	IntValue iv;
	iv.SetHashValue(1000);
	IntKey   ik(&iv); /*根据value产生key*/
	IntMap.Add(&iv);
	IntValue *v1=IntMap.Map(&ik);
	fprintf_debug(" Test OSHashTable %d\n",v1->GetHashValue());
 #endif

	/*do loop*/
	while(1){
		//fprintf_debug("This is main\n");
		OSThread::Sleep(1000);
	}
    
	return 0;
}
