#pragma once

extern "C" void outb  (int port, int value);
extern "C" void outw (int port, int value);
extern "C" void outl (int port, int value);
extern "C" int inb   (int port);
extern "C" int inw  (int port);
extern "C" int inl  (int port);

class IOPort
{
      int address;
   public:
      IOPort (int a) : address (a) {};

      // Output a byte to an I/O port.
      void outb (int val) const
       { 
	 ::outb (address, val); 
       };

      // Output a word to an I/O port.
      void outw (int val) const
       { 
	 ::outw (address, val); 
       };

      // Output a dword to an I/O port.
      void outl (int val) const
       { 
	 ::outl (address, val); 
       };

      // Read in a byte from an I/O port.
      int inb () const
       { 
	 return ::inb (address); 
       };

      // Read in a word from an I/O port.
      int inw () const
       { 
	 return ::inw (address); 
       };

      // Read in a dword from an I/O port.
      int inl () const
       { 
	 return ::inl (address); 
       };
};
