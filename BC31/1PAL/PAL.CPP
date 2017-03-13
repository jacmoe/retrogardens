#include <stdio.h>
#include <dos.h>
#include <conio.h>
#include "screen.h"

void main(void)
{

	// Save previous video mode:
	int oldmode = *(int*)MK_FP(0x40,0x49);

	// Create pointer to video memory:
	char far *screen = (char*)MK_FP(0xa000,0);

	// Set video mode
	setmode(0x13);

	// Draw a 16 by 16 matrix of squares in the default colours
	for(int square = 0; square < 256; square++)
		for(int y = 0; y < 12; y++)
			for(int x = 0; x < 20; x++)
				screen[square / 16 * 3840 + square % 16 * 20 + y * 320 + x] = square;

	// Wait for user to press a key:
	while(!kbhit());

	// Reset original video mode:
	setmode(oldmode);

}
