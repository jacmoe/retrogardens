//
// WHITEOUT Version 1.0
//
// Fills mode 13h video memory with a constant value of 0x0f,
// filling the display with white pixels
//
// Written by Christopher Lampton 3:11 a.m., July 25, 1993
//   for GARDENS OF IMAGINATION (Waite Group Press)

#include  <stdio.h>
#include	<dos.h>
#include	<conio.h>
#include  "screen.h"

void main(void)
{
  // Save previous video mode:

	int oldmode=*(int *)MK_FP(0x40,0x49);

  // Create pointer to video memory:

  char far *screen=(char *)MK_FP(0xa000,0);

  // Set mode 13h:

	setmode(0x13);

  // Fill video memory with a constant value:

  for (unsigned int i=0; i<64000; i++) screen[i]=0x0f;

  // Wait for user to press a key:

  while (!kbhit());

  // Reset original video mode:

  setmode(oldmode);
}
