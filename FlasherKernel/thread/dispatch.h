#pragma once

#include "thread/coroutine.h"

/*! \brief The dispatcher dispatches threads and puts the scheduler's
 *  decisions into action.
 *  \ingroup thread
 *
 *  The dispatcher manages the life pointer that refers to the currently
 *  active thread and performs the actual switching of processes.
 *  For single-core systems, a single life pointer is sufficient, as only a
 *  single thread can be active at any one time. On multi-core systems,
 *  every CPU core needs its own life pointer.
 */
class Dispatcher
{
private:
    Dispatcher(const Dispatcher &copy);
    Coroutine* m_life;
public:
	Dispatcher() {
		m_life = 0;
	}
	
	void go(Coroutine& first);
	void dispatch(Coroutine& next);
	Coroutine* active();
};
