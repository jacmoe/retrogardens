#include "screen.h"

byte* VGA = (byte*)0xA0000000L; /* points to video memory */

/* sets the video mode */
void set_mode(byte mode)
{
	union REGS regs;
	regs.h.ah = SET_MODE;
	regs.h.al = mode;
	int86(VIDEO_INT, &regs, &regs);
}

/* plots a pixel by writing directly to video memory */
void plot_pixel(int x, int y, byte color)
{
	VGA[y * SCREEN_WIDTH + x] = color;
}

/* plots a pixel by using BIOS function 0x0C (write dot) */
void plot_pixel_slow(int x, int y, byte color)
{
	union REGS regs;

	regs.h.ah = WRITE_DOT;
	regs.h.al = color;
	regs.x.cx = x;
	regs.x.dx = y;
	int86(VIDEO_INT, &regs, &regs);
}
