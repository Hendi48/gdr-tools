#pragma once

#include "sync/waitingroom.h"
#include "thread/scheduler.h"

/*! \brief Semaphore used for synchronization of threads.
 *  \ingroup ipc
 *
 *  The class Semaphore implements the concept of counting semaphores.
 *  The waiting list is provided by the base class Waitingroom.
 */
class Semaphore: public Waitingroom
{
    // Prevent copies and assignments
    Semaphore(const Semaphore&)            = delete;
    Semaphore& operator=(const Semaphore&) = delete;

private:
    unsigned int m_counter;

public:
    explicit Semaphore(unsigned int c) : m_counter(c) {}

    void p();
    void v();

    inline void wait() { p(); };
    inline void signal() { v(); };
};
