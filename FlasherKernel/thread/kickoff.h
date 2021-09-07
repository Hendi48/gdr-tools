#pragma once

#include "thread/coroutine.h"

#ifdef __cplusplus
extern "C" {
#endif
	/*! \brief Function to start a thread.
	 *
	 *  For the first activation of a thread, we need a "return address"
	 *  pointing to a function that will take care of calling C++ virtual
	 *  methods. For this purpose, we use this `kickoff()` function.
	 *
	 *  <b>Activating kickoff</b>
	 *
	 *  The context initialization via \ref toc_settle() not only initializes
	 *  the stack for the first thread change, but also pushes the address of
	 *  `kickoff()` as return address to the stack.
	 *  Consequently, the first execution of \ref toc_go() will start
	 *  execution by returning to the beginning of `kickoff()` .
	 *
	 *  This `kickoff()` function simply calls the \ref action() method on the
	 *  thread passed as parameter and, thus, resolves the virtual C++ method.
	 *
	 *  \note As this function is never actually called, but only executed by
	 *        returning from the co-routine's initial stack, it may never return.
	 *        Otherwise garbage values from the stack will be interpreted as
	 *        return address and the system might crash.
	 *
	 *  \param object Thread to be started
	 */
	void kickoff(Coroutine *object);
#ifdef __cplusplus
}
#endif
