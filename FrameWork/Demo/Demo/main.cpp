#include <stdio.h>

#include <stdlib.h>
#include "OS.h"
#include "Task.h"
#include <unistd.h>
#include<sys/types.h>

class TestTask:public Task
{
public:
    TestTask(){Signal(Task::kStartEvent);

               startTime=OS::Milliseconds();
              }
    virtual ~TestTask(){}
    virtual SInt64 Run()
    {
        EventFlags theEvents = this->GetEvents();
        if (theEvents & Task::kKillEvent){
            return -1;
        }
        static int a=0;
        fprintf(stderr,"hello %d\n",++a);
        return 3000;
    }
private:

    SInt64 startTime;
    SInt64 endTime;

};
class TestTask1:public Task
{
public:
    TestTask1(){Signal(
                    Task::kStartEvent);

                startTime=OS::Milliseconds();
               }
    virtual ~TestTask1(){}
    virtual SInt64 Run()
    {
        EventFlags theEvents = this->GetEvents();
        if (theEvents & Task::kKillEvent){
            return -1;
        }
        static int a=0;
        fprintf(stderr,"Test Task1 %d \n",++a);

        endTime=OS::Milliseconds();
        if(endTime-startTime>=10*1000){
            fprintf(stderr,"startTime %lld endTime %lld\n",startTime,endTime);
            Signal(Task::kKillEvent);
        }
        return 1;
    }

private:

    SInt64 startTime;
    SInt64 endTime;
};
int main(int argc,char *argv[])
{
    int pid=fork();
    if(pid>0){
        char *newName="WDog";
        fprintf(stderr,"%s %d\n",argv[0],strlen(argv[0]));
        memset(argv[0],0,strlen(argv[0]));
        memcpy(argv[0],newName,strlen(newName));
        getchar();
    }

    OS::Initialize();
    OSThread::Initialize();

    TaskThreadPool::AddThreads(1);
    Task *test=NULL;
    for(int i=0;i<5;++i){
        test=new TestTask;
    }

    while(1){
        getchar();
    }
    fprintf(stderr,"hello world");
    return 0;
}
