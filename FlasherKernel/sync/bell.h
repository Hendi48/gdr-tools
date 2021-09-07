#pragma once

#include "sync/waitingroom.h"
#include "device/watch.h"

/* Currently not implemented */

/*! \brief Synchronization object allowing to sleep for given timespan
 *  \ingroup ipc
 *
 *  A bell is a synchronization object enabling one or more threads to sleep for
 *  a particular timespan.
 */
class Bell : public Chain
{
    Bell(const Bell&)            = delete;
    Bell& operator=(const Bell&) = delete;

private:
    friend class Bellringer;

    unsigned int m_ticks;

public:
    Bell();

    void ring();
    static void sleep(unsigned int ms);
};
