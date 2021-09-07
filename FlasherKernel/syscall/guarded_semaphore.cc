#include "syscall/guarded_semaphore.h"
#include "interrupt/secure.h"

void Guarded_Semaphore::p()
{
    Secure sec;
    Semaphore::p();
}

void Guarded_Semaphore::v()
{
    Secure sec;
    Semaphore::v();
}
