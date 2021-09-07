#pragma once

#include "object/list.h"
#include "sync/bell.h"

/* Currently not implemented */

/*! \brief Manages and activates time-triggered activities.
 *  \ingroup ipc
 *
 *  The Bellringer is regularly activated and checks whether any of the bells should ring.
 *  The bells are stored in a List that is managed by the Bellringer.
 *  A clever implementation avoids iterating through the whole list for every iteration by
 *  keeping the bells sorted and storing delta times. This approach leads to a complexity
 *  of O(1) for the method called by the timer interrupt in case no bells need to be rung.
 */
class Bellringer: public List
{
    // Prevent copies and assignments
    Bellringer(const Bellringer&)            = delete;
    Bellringer& operator=(const Bellringer&) = delete;
public:
    Bellringer() {}

    void check();
    void job(Bell *bell, unsigned int ms);
    void cancel(Bell *bell);
    bool bell_pending();
};

extern Bellringer bellringer;
