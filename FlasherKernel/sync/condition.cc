#include "sync/condition.h"
#include "syscall/guarded_scheduler.h"

void Condition::wait()
{
    scheduler.block(*this);
}

void Condition::signal()
{
    Thread* thread;
    while ((thread = (Thread*) dequeue())) {
        scheduler.wakeup(*thread);
    }
}
