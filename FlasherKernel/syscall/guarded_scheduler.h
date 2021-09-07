#pragma once

#include "thread/scheduler.h"
#include "thread/thread.h"

class Guarded_Scheduler : public Scheduler
{
private:
    Guarded_Scheduler (const Guarded_Scheduler &copy);
public:
    Guarded_Scheduler () {}
	void ready(Thread& that);
	void exit();
	void kill(Thread& that);
	void resume();
};

extern Guarded_Scheduler scheduler;
