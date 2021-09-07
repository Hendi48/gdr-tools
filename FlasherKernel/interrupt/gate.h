#pragma once

#include "object/chain.h"

/*! \brief Class of objects that are capable of handling interrupts
 *  \ingroup interrupts
 *
 * All objects to be assigned in \ref Plugbox must be derived from this class.
 *
 * Each inheriting class must define the virtual method \ref Gate::trigger().
 */
class Gate : public Chain {
private:
    Gate(const Gate &copy);
    bool m_queued;
public:
	Gate() { m_queued = false; };

	virtual bool prologue() = 0;
	virtual void epilogue() {
	}

	void queued(bool q) { m_queued = q; };

	bool queued() {	return m_queued; };
};
