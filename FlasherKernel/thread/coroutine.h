#pragma once

#include "machine/toc.h"

/* Implementation of the coroutine concept.
 * The constructor sets up the coroutine's initial context.
 * go() activates the coroutine for the first time. All the following
 * coroutine switches should be done via resume().
 * In order to save the context during a switch, each coroutine object
 * has a toc structure, which is used to save the non-volatile registers.
 */
class Coroutine
{
private:
	Coroutine(const Coroutine &copy);
	
	toc m_regs;
public:
	Coroutine(void* tos);
	void go();
	void resume(Coroutine& next);
	virtual void action() = 0;
};
