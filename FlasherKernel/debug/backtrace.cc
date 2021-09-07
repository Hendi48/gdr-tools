#include "debug/backtrace.h"
#include "device/cgastr.h"

#include <cinttypes>

extern "C" void** get_base_pointer();

void Backtrace::gather()
{
    void** next = get_base_pointer();
    int i = 0;
    while (i < 7 && (uint64_t) next >= 0x100000 && (uint64_t) next < 0x200000) {
        trace[i++] = *(next + 1);
        next = (void**) *next;
    }
    trace[i] = 0;
}

void Backtrace::print()
{
    kout << "Backtrace:" << endl;
    for (int i = 0; i < 8 && trace[i]; i++) {
        kout << trace[i] << endl;
    }
    kout.flush();
}
