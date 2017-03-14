// --------- keyboard.cpp

#include "jmkbd.h"

void interrupt (*Keyboard::oldkbint)(...);

unsigned char Keyboard::scancode;
bool Keyboard::kys[128];

// --- ascii offset to scancode table
SHORT Keyboard::scancodes[256] = {
   0,   0,   0,   0,   0,   0,   0,  15,
  14,   0,   0,   0,   0,  28,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   1,   0,   0,   0,   0,
  57,   0,   0,   0,   0,   0,   0,  40,
   0,   0,  55,  78,  51,  12,  52,  53,
  11,   2,   3,   4,   5,   6,   7,   8,
   9,  10,   0,  39,   0,  13,   0,   0,
   0,  30,  48,  46,  32,  18,  33,  34,
  35,  23,  36,  37,  38,  50,  49,  24,
  25,  16,  19,  31,  20,  22,  47,  17,
  45,  21,  44,  26,  43,  27,   0,   0,
  41,  30,  48,  46,  32,  18,  33,  34,
  35,  23,  36,  37,  38,  50,  49,  24,
  25,  16,  19,  31,  20,  22,  47,  17,
  45,  21,  44,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,
  56,   0,   0,  59,  60,  61,  62,  63,
  64,  65,  66,  67,  68,  87,  88,  71,
  72,  73,  74,  75,   0,  77,  78,  79,
  80,  81,  82,  83
};

Keyboard::Keyboard()
{
	oldkbint = getvect(9);
	setvect(9, newkbint);
}
Keyboard::~Keyboard()
{
	setvect(9, oldkbint);
}
void interrupt Keyboard::newkbint(...)
{
	unsigned char kbportin;
	scancode = inp(0x60);
	kbportin = inp(0x61);
	outp(0x61, (kbportin | 0x80));
	outp(0x61, kbportin);
	outp(0x20, 0x20);
	kys[scancode & 0x7f] = ((scancode & 0x80) == 0);
}
bool Keyboard::wasPressed(int ky)
{
	SHORT key = scancodes[ky];
	if (kys[key])	{
		while (kys[key])
			;
		return true;
	}
	return false;
}

