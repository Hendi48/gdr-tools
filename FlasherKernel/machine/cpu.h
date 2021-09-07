#pragma once

// Implemented in cpu.asm
extern "C" void int_enable ();
extern "C" void int_disable ();
extern "C" void cpu_idle ();
extern "C" void cpu_halt ();

/*! \brief Implements an abstraction for CPU internals.
 *
 * These internals include functions to enable/disable interrupts globally,
 * and to halt the processor.
 */
class CPU
{
private:
    CPU(const CPU &copy);
public:
    CPU() {}
    // Enable (hardware-)interrupts
    inline void enable_int ()
    {
        int_enable ();
    }

    // Disable/ignore interrupts
    inline void disable_int ()
    {
        int_disable ();
    }

    // Stop processor until the next interrupt occurs
    inline void idle ()
    {
        cpu_idle ();
    }

    // Stop processor
    inline void halt ()
    {
        cpu_halt ();
    }
};
 
extern CPU cpu;
