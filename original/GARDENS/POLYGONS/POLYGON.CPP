
// POLYGON.CPP Version 1.0
// Draws an unfilled polygon on the mode 13h display
//
// Written by Christopher Lampton
// for GARDENS OF IMAGINATION (Waite Group Press, 1994)
//

#include  <stdio.h>
#include	<conio.h>
#include  <dos.h>
#include  "screen.h"
#include	"bresnham.h"

void main(int argc,char *argv[])
{
	// Create pointer to video memory:

	char far *screen=(char far *)MK_FP(0xa000,0);

	// Save previous video mode:

	int oldmode=*(int *)MK_FP(0x40,0x49);

	// Put display in mode 13h:

	setmode(0x13);

	// Clear display:

	cls(screen);

	// Draw polygon:

	linedraw(17,24,107,68,15,screen);
	linedraw(107,68,190,79,15,screen);
	linedraw(190,79,150,180,15,screen);
	linedraw (150,180,17,24,15,screen);

	while (!kbhit());

	// Restore old video mode:

	setmode(oldmode);
}
