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

#define SC_INDEX			0x03c4		/* VGA sequence controller */
#define SC_DATA				0x03c5
#define PALETTE_INDEX		0x03c8		/* VGA digital-to-analog converter */
#define PALETTE_DATA		0x03c9
#define GC_INDEX			0x03ce		/* VGA graphics controller */
#define GC_DATA				0x03cf
#define CRTC_INDEX			0x03d4		/* VGA CRT controller */
#define CRTC_DATA			0x03d5
#define INPUT_STATUS_1		0x03da

#define MAP_MASK			0x02		/* Sequence controller registers */
#define ALL_PLANES			0xff02
#define MEMORY_MODE			0x04

#define LATCHES_ON			0x0008		/* Graphics controller registers */
#define LATCHES_OFF			0xff08

#define HIGH_ADDRESS		0x0C		/* CRT controller registers */
#define LOW_ADDRESS			0x0D
#define UNDERLINE_LOCATION	0x14
#define MODE_CONTROL		0x17

#define DISPLAY_ENABLE		0x01		/* VGA input status bits */
#define VRETRACE			0x08

#define VIDEO_MEM			0xA0000000L	/* video memory location */

#define SCREEN_WIDTH		320			/* mode 0x13 width in pixels */
#define SCREEN_HEIGHT		200			/* mode 0x13 height in pixels */
#define SCREEN_SIZE			(word)(SCREEN_WIDTH*SCREEN_HEIGHT)
#define NUM_COLORS			256			/* mode 0x13 number of colors */

#define BITMAP_WIDTH		32
#define BITMAP_HEIGHT		25
#define ANIMATION_FRAMES	24
#define TOTAL_FRAMES		140
#define VERTICAL_RETRACE

#define sgn(x) ((x < 0) ? -1 : ((x > 0) ? 1 : 0))	/* macro to return sign of number */

typedef unsigned char byte;
typedef unsigned short word;
typedef unsigned long dword;

typedef struct tagBITMAP		/* the structure for a bitmap. */
{
	word width;
	word height;
	byte palette[256*3];
	byte *data;
} BITMAP;

/* skip bytes in a file */
void fskip(FILE *fp, int num_bytes);

/* sets the video mode */
void set_mode(byte mode);

/* resets VGA mode 0x13 to unchained mode to access all 256K of memory */
void set_unchained_mode(void);

/* 	switches the pages at the appropriate time and waits for the vertical retrace. */
void page_flip(word *page1, word *page2);

/* displays a memory buffer on the screen */
void show_buffer(byte *buffer);

/* loads a bitmap file into memory */
void load_bmp(char *file, BITMAP *b);

/* sets all 256 colors of the palette */
void set_palette(byte *palette);

/* plots a pixel in unchained mode */
void plot_pixel_um(int x, int y, byte color);

/* plots a pixel by writing directly to video memory */
void plot_pixel(int x, int y, byte color);

/* plots a pixel by writing directly to video memory */
/* buffered version */
void plot_pixel_buf(int x, int y, byte color, byte* buffer);

/* draws a line using Bresenham's line-drawing algorithm
   no multiplication or division */
void line(int x1, int y1, int x2, int y2, byte color);

/* draws a line using Bresenham's line-drawing algorithm
   no multiplication or division */
/* buffered version */
void line_buf(int x1, int y1, int x2, int y2, byte color, byte* buffer);

/* draw a vertical line on x, from y1 to y2 */
/* buffered version */
void vert_line_buf(int x, int y1, int y2, byte color, byte* buffer);

/* draws a rectangle */
void rect(int left, int top, int right, int bottom, byte color);

/* draws a rectangle */
/* buffered version */
void rect_buf(int left, int top, int right, int bottom, byte color, byte* buffer);

/* draws a filled rectangle */
void rect_fill(int left, int top, int right, int bottom, byte color);


#endif
