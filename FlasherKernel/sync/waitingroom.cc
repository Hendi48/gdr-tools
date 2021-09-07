#include "sync/waitingroom.h"
#include "syscall/guarded_scheduler.h"

Waitingroom::~Waitingroom()
{
    Thread* customer;
    while ((customer = (Thread*) dequeue())) {
        scheduler.wakeup(*customer);
    }
}

void Waitingroom::remove(Thread *customer)
{
    Queue::remove(customer);
}
