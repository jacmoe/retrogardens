#include <stdio.h>
#include <dos.h>
#include <conio.h>
#include "screen.h"

void main(void)
{
	// Set video mode
	set_mode(VGA_256_COLOR_MODE);

	byte* screen = (byte*)VIDEO_MEM;

	// Draw a 16 by 16 matrix of squares in the default colours
	for(int square = 0; square < 256; square++)
		for(int y = 0; y < 12; y++)
			for(int x = 0; x < 20; x++)
				screen[square / 16 * 3840 + square % 16 * 20 + y * 320 + x] = square;

	// Wait for user to press a key:
	while(!kbhit());

	// Reset original video mode:
	set_mode(TEXT_MODE);

}
