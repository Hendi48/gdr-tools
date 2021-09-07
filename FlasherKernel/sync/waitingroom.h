#pragma once

#include "object/queue.h"
#include "thread/thread.h"

/*! \brief List of threads waiting for an event.
 *  \ingroup ipc
 *
 *
 *  The class Waitingroom implements a list of threads that all wait for one
 *  particular event.
 *
 *  The destructor should be virtual to properly cleanup derived classes.
 */
class Waitingroom : public Queue
{
    // Prevent copies and assignments
    Waitingroom(const Waitingroom&)            = delete;
    Waitingroom& operator=(const Waitingroom&) = delete;

public:
    Waitingroom() {}
    virtual ~Waitingroom();
    virtual void remove(Thread *customer);
};
