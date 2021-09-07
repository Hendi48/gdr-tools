#pragma once

class Backtrace {
private:
    void* trace[8];

    void gather();
public:
    inline Backtrace() __attribute__((always_inline)) {
        gather();
    };

    void print();
};
