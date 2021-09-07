#pragma once

#include "object/strbuf.h"

/*! \brief The class OutputStream corresponds, essentially, to the class ostream
 *  from the C++ IO-Stream library.
 *
 *  As relying on the method \ref Stringbuffer::put() is quite cumbersome when
 *  not only printing single characters, but numbers and whole strings, the
 *  class OutputStream provides a convenient way of composing output of variables of
 *  varying data types.
 *  Therefore, OutputStream implements shift operators `operator<<`` for various
 *  data types (similar to those known from the C++ IO-Stream library)
 *
 *  OutputStream's public methods/operators all return a reference to the object
 *  they are called on (i.e. `*this`). Returning `*this` allows chaining those
 *  stream operators in a single expression, such as
 *  <tt>kout << "a = " << a</tt>;
 *
 *  At this point in time, OutputStream implements `operator<<`` for chars, strings
 *  and whole numbers. An additional `operator<<` allows using manipulators
 *  whose detailed description is given below.
 */
class OutputStream : public Stringbuffer    
{
private:
	OutputStream(const OutputStream &copy);
private:
	unsigned int m_base, m_size;

	void print_base_2_8_16(unsigned long number);
public:
	OutputStream(){ m_base = 10; };

	OutputStream& operator<< (unsigned char c);
	OutputStream& operator<< (char c);
    OutputStream& operator<< (unsigned short number);
    OutputStream& operator<< (short number);
    OutputStream& operator<< (unsigned int number);
    OutputStream& operator<< (int number);
    OutputStream& operator<< (unsigned long number);
    OutputStream& operator<< (long number);
    OutputStream& operator<< (void* pointer);
    OutputStream& operator<< (char* text);
    OutputStream& operator<< (const char* text);
    OutputStream& operator<< (OutputStream& (*func) (OutputStream&));
    
    void set_base(unsigned int base);
    void set_size(unsigned int size);
};

// Manipulators

// Inserts a new line
OutputStream& endl (OutputStream& os);

// Selects the binary numeric system
OutputStream& bin (OutputStream& os);
        
// Selects the octal numeric system
OutputStream& oct (OutputStream& os);
        
// Selects the decimal numeric system
OutputStream& dec (OutputStream& os);
        
// Selects the hexadecimal numeric system
OutputStream& hex (OutputStream& os);
