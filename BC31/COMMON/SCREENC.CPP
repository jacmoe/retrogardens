//-------+---------+---------+---------+---------+---------+---------+---------+
// Copyright 1991-1992 Betz Associates. All rights reserved.
//
// File Name: SCREEN.CPP
// Project:   Flights of Fantasy
// Creation:  January 18, 1992
// Author:    Mark Betz (MB)
//
// Machine:   IBM PC and Compatibles
//
// Includes:  dos.h, mem.h, stdio.h, types.h, screen.h, bresenham.h
//
//-------+---------+---------+---------+---------+---------+---------+---------+
// Change History
// ------ -------
//
//      Date            Rev.	Author		Purpose
//      ----            ----    ------      -------
//      1-18-1992       1.0     MB          Initial development
//      8-29-1992       1.1b    MB          first beta
//      9-26-1992       1.1     MB          publication release
//
// Description
// -----------
//      Function definitions for control of the VGA adapter and basic bitmapped
//      graphics operations.
//-------+---------+---------+---------+---------+---------+---------+---------+

#include <dos.h>
#include <mem.h>
#include <stdio.h>

#ifndef __TYPES__
	#include "types.h"
#endif
#ifndef __SCREEN__
	#include "screen.h"
#endif
#ifndef __BRESNHAM__
    #include "bresnham.h"
#endif

static const int DacWrite = 0x3C8;        // DAC write index register
static const int DacRead  = 0x3C7;        // DAC read index register
static const int DacData  = 0x3C9;        // DAC data register
static const int inputStatus1 = 0x3DA;    // Port addr. of VGA input status 1 reg.
static int graphSeg = 0xa000;            // graphics buffer segment
static int graphOff = 0;                 // graphics buffer offset

static int palette_size = 768;            // size of vga palette

// sets the sets the segment and offset of the 64000 byte graphics buffer,
// which defaults to a000h:0000h if parameter theBuffer is NULL

void SetGfxBuffer( void far* theBuffer )
    {
    if ( theBuffer == NULL )
       {
       graphSeg = 0xa000;
       graphOff = 0;
       }
    else
       {
       graphSeg = FP_SEG( theBuffer );
       graphOff = FP_OFF( theBuffer );
       }
    }

// returns a void type pointer to video memory at pixel coordinate x,y

void far *XyToPtr(int x, int y)
	{
	return(( char* )MK_FP( graphSeg, ( graphOff + ( y * 320 ) + x )));
	}

// set the pixel at coordinate x,y to color
// This could be translated into assembler, but is probably fast enough

void WritePixel( int x, int y, char color )
	{
	*( char far* )MK_FP( graphSeg, ( graphOff + ( y * 320 ) + x )) = color;
	}

// GetImage() grabs the pixel values in the rectangle marked by (x1,y1) on
// the top left, and (x2,y2) on the bottom right. The data is placed in buffer.
//
// NOTE: This function probably needs translating into assembler

void GetImage( int x1, int y1, int x2, int y2, char *buffer )
	{
	int i;
	int deltaY = ( y2-y1 ) + 1;
	int xdim = ( x2-x1 ) + 1;

	for ( i = 0; i < deltaY; i++ )
		{
		_fmemcpy(( void* )&buffer[ i * xdim ], XyToPtr( x1,( y1 + i )), xdim );
		}
	}

//-------+---------+---------+---------+---------+---------+---------+---------+
// PutImage() copies the data in *buff to a rectangular area of the screen
// marked by (x1,y1) on the top left, and (x2,y2) on the bottom right.
//
// NOTE: This function probably needs translating into assembler
//-------+---------+---------+---------+---------+---------+---------+---------+

void PutImage( int x1, int y1, int x2, int y2, char *buffer )
	{
	int ydim = ( y2-y1 ) + 1;
	int xdim = ( x2-x1 ) + 1;
    putimage( x1, y1, xdim, ydim, buffer, MK_FP(graphSeg, graphOff));
	}

// this function calls the linedraw function in BRESNHAM.ASM. This function
// is required as a gateway because the modules calling Line() do not know
// where the graphics buffer is actually located. The location of the
// buffer is stored in the static locations graphSeg and graphOff in this
// module, and set via calls to SetGfxBuffer(). An alternative would be to
// make graphSeg and graphOff global to the program by removing the static
// storage specifier, but this should only be done if the overhead of the
// extra far call involved in using this gateway is proven to be significant.

void Line(int x1, int y1, int x2, int y2, char color)
    {
    linedraw( x1, y1, x2, y2, color, (char far*)MK_FP( graphSeg, graphOff ));
    }

// reads the dac data in palette into the supplied
// palette structure.

void ReadPalette(int start, int number, char far* palette)
	{
	int i, j;
	if ((start>256) | (start<0) | ((start+number)>256))
		return;
	for ( i = start; i < ( start + number ); i++ )
		{
        j = i * 3;
		asm cli;
		outportb(DacRead,i);
		palette[ j ] = inportb(DacData);
		palette[ ++j ] = inportb(DacData);
		palette[ ++j ] = inportb(DacData);
		asm sti;
		}
	}

// clears a range of palette registers to zero

void ClrPalette(int start, int number)
	{
	int i;
	if ((start>256) | (start<0) | ((start+number)>256))
		return;
	for (i=start;i<(start+number);i++)
		{
		asm cli;
		outportb(DacWrite,i);
		outportb(DacData,0);
		outportb(DacData,0);
		outportb(DacData,0);
		asm sti;
		}
	}

// WaitVbi() waits twice for the vertical blanking interval. Once to make
// sure any current vbi is completed, and once for the start of the next vbi

void WaitVbi()
	{
	asm mov dx, inputStatus1;
    test_1:
	asm	{
		in al, dx
		test al,0x8
		jnz test_1
		}
	test_2:
	asm	{
		in al, dx
		test al,0x8
		jz test_2
		}
	}

// clear the graphics screen to the color provided

void ClearScr(char color)
	{
	asm {
		mov es, graphSeg
		mov di, graphOff
		mov ah, color
		mov al, color
		mov cx, 32000
		rep stosw
		}
	}

// fill the rectangular area bounded by (tlx,tly)(brx,bry) with color.
// tlx = top left x, tly = top left y, brx = bottom right x, bry = bottom
// right y.
//
// This function needs to be recoded in assembler

void BarFill(int x1, int y1, int x2, int y2, char color)
	{
	int row;
	void far *line;
	for ( row = y1; row <= y2; row++ )
		{
		line = MK_FP( graphSeg, ( graphOff + (( row * 320 ) + x1 )));
		_fmemset( line, color, (( x2-x1 ) + 1));
		}
	}

// calculates and returns the buffer size necessary to hold the image bounded
// by x1, y1 at top left, and x2, y2 at lower right.

int BufSize( int x1, int y1, int x2, int y2 )
    {
    int size;
    size = ((x2 - x1) + 1) * ((y2 - y1) + 1);
    return( size );
    }