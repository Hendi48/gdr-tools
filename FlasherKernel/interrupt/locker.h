#pragma once

#include "machine/cpu.h"
#include "device/cgastr.h"
#include "debug/backtrace.h"

/**
 * This class implements a lock variable. It doesn't implement any
 * waiting/protection logic by itself.
 */
class Locker
{
private:
    Locker(const Locker &copy);

	bool m_locked;
public:
	Locker() : m_locked(false) {};

	void enter() {
		if (m_locked) {
			Backtrace trace;
			kout << "Tried to enter Locker twice!!!" << endl;
			trace.print();
			cpu.halt();
		}
		m_locked = true;
	}

	void retne() {
		if (!m_locked) {
			Backtrace trace;
			kout << "Tried to leave Locker twice!!!" << endl;
			trace.print();
			cpu.halt();
		}
		m_locked = false;
	}

	inline bool avail() {
		return !m_locked;
	}
};
