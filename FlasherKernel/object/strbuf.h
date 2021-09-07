#pragma once

/*! \brief The class Stringbuffer composes single characters into a longer text
 *  that can be processed as a single block.
 *
 *  To make Stringbuffer as versatile as possible, the class doesn't make
 *  assumptions about the underlying hardware, nor the meaning of
 *  "processing". When flush() is called (i.e., either on explicit request or
 *  once the buffer is full), the buffer is processed and emptied. 
 *  To be hardware independent, flush() is to be implemented by the
 *  derived classes.
 *
 *  \par Notes
 *  Reason for the existence of this class is that generating longer texts is
 *  often implemented by assembly of small fragments (such as single characters
 *  or numbers).
 *  However, writing such small fragments directly to (for example) screen is
 *  quite inefficient (e.g., due to the use of IO ports, syscalls, or locks) and
 *  can be improved drastically by delaying the output step until the assembly
 *  is finished (or the buffer runs full).
 */
class Stringbuffer
{
private:
	Stringbuffer(const Stringbuffer &copy);
protected:
	char buf[128];
	int index;
public:
	Stringbuffer() {};
	void put(char c);
	virtual void flush() = 0;   
};
