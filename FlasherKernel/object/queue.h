#pragma once

#include "object/chain.h"

/*! \brief Simple singly-linked queue for arbitrary objects.
 *
 * Queue is implemented by a head-object (Queue) and next-pointers embedded in the
 * queued objects.
 * 
 * Note: Unlike most queue implementations, tail doesn't point to the last
 * element, but to the last element's next pointer (or head initially).
 */
class Queue
{
private:
    Queue(const Queue &copy);

protected:
    Chain* head;
    Chain** tail;

public:
    Queue () { head = 0; tail = &head; }
    void enqueue (Chain* item);
    // Removes and returns the first element (null if empty)
    Chain* dequeue ();
    void remove (Chain*);
};
