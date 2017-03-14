#ifndef __SCREEN_H__
#define __SCREEN_H__

#include <stdio.h>
#include <stdlib.h>
#include <dos.h>

#define VIDEO_INT			0x10		/* the BIOS video interrupt. */
#define WRITE_DOT			0x0C		/* BIOS func to plot a pixel */
#define SET_MODE			0x00		/* BIOS func to set video mode */
#define VGA_256_COLOR_MODE	0x13		/* used to set 256 color mode */
#define TEXT_MODE			0x03		/* used to set 80x25 text mode */

#define VIDEO_MEM			0xA0000000L	/* video memory location */

#define SCREEN_WIDTH		320			/* mode 0x13 width in pixels */
#define SCREEN_HEIGHT		200			/* mode 0x13 height in pixels */
#define NUM_COLORS			256			/* mode 0x13 number of colors */

typedef unsigned char byte;
typedef unsigned short word;

/* sets the video mode */
void set_mode(byte mode);

/* plots a pixel by writing directly to video memory */
void plot_pixel(int x, int y, byte color);

/* plots a pixel by using BIOS function 0x0C (write dot) */
void plot_pixel_slow(int x, int y, byte color);

#endif
