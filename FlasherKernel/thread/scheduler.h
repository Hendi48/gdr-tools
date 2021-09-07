#pragma once

#include "thread/dispatch.h"
#include "thread/thread.h"
#include "object/queue.h"
        
class Scheduler : public Dispatcher    
{
private:
	Scheduler (const Scheduler &copy);
	Queue m_ready;
public:
	Scheduler() : Dispatcher() {
	}

	void ready(Thread& that);
	void schedule();
	void exit();
	void kill(Thread& that);
	// Preempts the current thread
	void resume();

    void block(Waitingroom& waitingroom);
    void wakeup(Thread& thread);
};
