#pragma once

#include "interrupt/guard.h"

/*! \brief A handy interface to protect critical sections
 *  \ingroup interrupts
 *
 * So if you \ref Guard::enter() "enter" a critical section by constructing
 * an instance of \ref Secure and \ref Guard::leave() "leave" it again in
 * the destructor, you can easily mark critical code areas as
 * follows:
 *
 *  \code{.cpp}
 *	// non-critical section
 *	...
 *	{
 *	    Secure section;
 *	    // critical section
 *	    ...
 *	}
 *	// non-critical section
 *	\endcode
 */
class Secure
{
private:
    Secure (const Secure &copy);
public:
    Secure() {
    	guard.enter();
    }

	~Secure() {
		guard.leave();
	}
};
