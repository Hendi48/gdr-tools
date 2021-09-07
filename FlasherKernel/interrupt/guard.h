#pragma once

#include "object/queue.h"
#include "interrupt/gate.h"
#include "interrupt/locker.h"

/*! \brief Synchronizes the kernel with interrupts using the Prologue/Epilogue Model
 *  \ingroup interrupts
 *
 * The Guard is used to synchronize between "normal" core activities
 * and interrupt handling routines.
 * For this purpose, \ref Guard has to contain one ore more \ref Queue "queues",
 * in which gate objects can be added. This is necessary if the critical
 * section is occupied at the time when an interrupt occurs, and the `epilogue()`
 * method cannot be executed immediately. The queued epilogues are processed
 * when leaving the critical section.
 */
class Guard : public Locker
{
private:
    Guard (const Guard &copy);
    Queue m_epilogues;
public:
    Guard () {}

	void leave();
	void relay(Gate* item);
};
 
extern Guard guard;
