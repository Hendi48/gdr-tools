#pragma once

#include "sync/waitingroom.h"
#include "thread/scheduler.h"

class Condition : public Waitingroom
{
    // Prevent copies and assignments
    Condition(const Condition&)            = delete;
    Condition& operator=(const Condition&) = delete;

public:
    explicit Condition() {}

    void wait();
    void signal();
};
