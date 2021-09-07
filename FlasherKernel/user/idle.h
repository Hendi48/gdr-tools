#pragma once

#include "thread/thread.h"

class IdleThread : public Thread
{
private:
    IdleThread (const IdleThread &copy);
	char m_stack[2048];
public:               
    IdleThread() : Thread(&m_stack[1024]) {}
    void action ();
};

extern IdleThread idle_thread;
