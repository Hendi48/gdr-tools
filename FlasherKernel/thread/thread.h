#pragma once

#include "object/chain.h"
#include "thread/coroutine.h"

class Waitingroom;

class Thread : public Chain, public Coroutine
{
private:
	Thread (const Thread &copy);

	Waitingroom* m_waitingroom;
public:
	Thread(void* tos) : Coroutine(tos), m_waitingroom(nullptr) {}

	void waiting_in(Waitingroom* waitingroom)
	{
		m_waitingroom = waitingroom;
	}

	Waitingroom* waiting_in()
	{
		return m_waitingroom;
	}
};
