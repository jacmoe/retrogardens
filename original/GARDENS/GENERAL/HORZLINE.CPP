//
// HORZLINE Version 1.0
//
// Draws a horizontal line across the mode 13h display
//
// Written by Christopher Lampton 9:20 p.m., July 25, 1993
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

  // Set starting coordinates for line:

  int startx=10;
  int starty=100;

  // Calculate video memory offset of coordinates:

  int offset=starty*320+startx;

  // Draw the line:

  for (int i=0; i<300; i++) screen[offset+i]=15;

  // Wait for user to press a key:

  while (!kbhit());

  // Reset original video mode:

  setmode(oldmode);
}
