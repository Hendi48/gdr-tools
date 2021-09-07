#include "object/o_stream.h"

OutputStream& OutputStream::operator<< (unsigned char c) {
	put(c);
	return *this;
}

OutputStream& OutputStream::operator<< (char c) {
	put(c);
	return *this;
}

OutputStream& size16 (OutputStream& os) {
	os.set_size(16);
	return os;
}

OutputStream& size32 (OutputStream& os) {
	os.set_size(32);
	return os;
}

OutputStream& size64 (OutputStream& os) {
	os.set_size(64);
	return os;
}

OutputStream& OutputStream::operator<< (unsigned short number) {
	return *this << size16 << (unsigned long) number << size64;
}

OutputStream& OutputStream::operator<< (short number) {
	return *this << size16 << (long) number << size64;
}

OutputStream& OutputStream::operator<< (unsigned int number) {
	return *this << size32 << (unsigned long) number << size64;
}

OutputStream& OutputStream::operator<< (int number) {
	return *this << size32 << (long) number << size64;
}

static const char hex_chars[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

void OutputStream::print_base_2_8_16(unsigned long number) {
	int shift = 4;
	switch (m_base) {
		case 2:
			shift = 1;
			break;
		case 8:
			shift = 3;
			break;
	}
	unsigned long num = number;
	unsigned long pow = 1;
	while (num >= m_base) {
	    pow <<= shift;
	    num >>= shift;
	}
	num = number;
	while (pow) {
		unsigned long digit = num / pow;
		put(hex_chars[digit]);
		num -= digit * pow;
		pow >>= shift;
	}
}

OutputStream& OutputStream::operator<< (unsigned long number) {
	if (m_base == 10) {
		unsigned long num = number;
		unsigned long pow10 = 1;
		while (num >= 10) {
		    pow10 *= 10;
		    num /= 10;
		}
		num = number;
		while (pow10) {
			unsigned long digit = num / pow10;
			put('0' + digit);
			num -= digit * pow10;
			pow10 /= 10;
		}
	} else {
		if (m_size == 16) {
			number = (unsigned short) number;
		} else if (m_size == 32) {
			number = (unsigned int) number;
		}
		print_base_2_8_16(number);
	}

	return *this;
}

OutputStream& OutputStream::operator<< (long number) {
	if (m_base == 10) {
		if (number < 0) {
			put('-');
		}
		unsigned long num = number < 0 ? -number : number;
		return *this << num;
	} else {
		return *this << (unsigned long) number;
	}
}

OutputStream& OutputStream::operator<< (void* pointer) {
	unsigned int old = m_base;
	*this << hex << (unsigned long) pointer;
	m_base = old;
	return *this;
}

OutputStream& OutputStream::operator<< (char* text) {
	while (*text) {
		put(*text++);
	}
	return *this;
}

OutputStream& OutputStream::operator<< (const char* text) {
	while (*text) {
		put(*text++);
	}
	return *this;
}

OutputStream& OutputStream::operator<< (OutputStream& (*func) (OutputStream&)) {
	return func(*this);
}

void OutputStream::set_base(unsigned int base) {
	m_base = base;
}

void OutputStream::set_size(unsigned int size) {
	m_size = size;
}

// Manipulators

OutputStream& endl (OutputStream& os) {
	return os << '\n';
}

OutputStream& bin (OutputStream& os) {
	os.set_base(2);
	return os;
}

OutputStream& oct (OutputStream& os) {
	os.set_base(8);
	return os;
}

OutputStream& dec (OutputStream& os) {
	os.set_base(10);
	return os;
}

OutputStream& hex (OutputStream& os) {
	os.set_base(16);
	return os;
}
