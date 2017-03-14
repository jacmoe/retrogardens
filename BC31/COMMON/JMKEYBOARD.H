// -------- jmkeyboard.h

#ifndef JMKEYBOARD_H
#define JMKEYBOARD_H

#include <dos.h>
//#include "consts.h"

/* start consts header */
typedef unsigned short WORD;	// 16 bits
typedef short SHORT;			// 16 bits
typedef long LONG;				// 32 bits
typedef unsigned long DWORD;	// 32 bits

typedef unsigned short int bool;
const bool false = 0;
const bool true  = 1;
/* end consts header */

const int homekey  = 71+128;
const int pgupkey  = 73+128;
const int endkey   = 79+128;
const int pgdnkey  = 81+128;
const int f1key    = 59+128;
const int f2key    = 60+128;
const int f3key    = 61+128;
const int f4key    = 62+128;
const int f5key    = 63+128;
const int f6key    = 64+128;
const int f7key    = 65+128;
const int f8key    = 66+128;
const int f9key    = 67+128;
const int f10key   = 68+128;
const int f11key   = 87+128;
const int f12key   = 88+128;
const int uparrow  = 72+128;
const int dnarrow  = 80+128;
const int rtarrow  = 77+128;
const int lfarrow  = 75+128;
const int inskey   = 82+128;
const int delkey   = 83+128;
const int altkey   = 56+128;
const int pluskey  = 78+128;
const int minuskey = 74+128;
const int esckey   = 27;

class Keyboard	{
    static void interrupt (*oldkbint)(...);
	static void interrupt newkbint(...);
	static bool kys[128];
	static SHORT scancodes[256];
	static unsigned char scancode;
public:
	Keyboard();
	~Keyboard();
	bool wasPressed(int ky);
	bool isKeyDown(int ky);
};
inline bool Keyboard::isKeyDown(int ky)
{
	return kys[scancodes[ky]];
}
#endif

