#include "sync/semaphore.h"
#include "syscall/guarded_scheduler.h"

void Semaphore::p()
{
    if (m_counter > 0) {
        m_counter--;
    } else {
        scheduler.block(*this);
    }
}

void Semaphore::v()
{
    Thread* thread = (Thread*) dequeue();
    if (thread) {
        scheduler.wakeup(*thread);
    } else {
        m_counter++;
    }
}
