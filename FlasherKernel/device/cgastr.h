#pragma once

#include "object/o_stream.h"
#include "machine/cgascr.h"

/*! \brief Output text (form different data type sources) on screen.
 */
class CGAStream : public CGAScreen, public OutputStream
{
private:
	CGAStream(CGAStream &copy);
public:
	CGAStream(){};
	void flush();
};
 
extern CGAStream kout;
