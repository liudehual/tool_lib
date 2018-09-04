#include "Job.h"
Engine Job::eng;

Job::Job():kKill(false)
{
	jobElem.SetEnclosingObject(this);
}
Job::~Job()
{
}
int Job::Run()
{
	eng.start();
	fprintf(stderr,"Job::Run Exit\n");
	return 1;
}

