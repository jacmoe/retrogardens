// BRESNHAM.H
// include file containing prototype for assembler implementation of
// Bresenham's line drawing algorithm in BRESNHAM.ASM

#ifndef __BRESNHAM__
#define __BRESNHAM__

extern "C" {
	void linedraw(int,int,int,int,int,char far *);
}

#endif