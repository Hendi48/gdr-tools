#pragma once

#include "sync/semaphore.h"

class Guarded_Semaphore : public Semaphore
{
private:
    Guarded_Semaphore (const Guarded_Semaphore &copy);
public:
    Guarded_Semaphore(unsigned int c) : Semaphore(c) {}

	void p();
    void v();
    inline void wait() { p(); };
    inline void signal() { v(); };
};
