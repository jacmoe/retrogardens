
// WALLDEMO.CPP
//
// Calls raycasting function to draw mono-colored
// view of maze.
//
// Written by Christopher Lampton for
// Gardens of Imagination (Waite Group Press,1994)

#include  <stdio.h>
#include	<dos.h>
#include	<conio.h>
#include  <stdlib.h>
#include  <math.h>
#include  "screen.h"
#include	"pcx.h"
#include  "wallcast.h"

map_type map={
 { 2, 2, 2, 2, 7, 4, 7, 2, 2, 2, 2, 2, 2, 2, 2, 2},
 { 9, 2, 0, 2, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 2},
 { 9, 0, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 2},
 { 9, 0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 2},
 { 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 0, 0, 0, 2},
 { 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 0, 0, 0, 2},
 { 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 9, 9, 0, 0, 2},
 { 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 2},
 { 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 9, 9, 0, 0, 2},
 { 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 0, 0, 0, 2},
 { 2, 2, 2, 0, 0, 0, 2, 2, 0, 2, 0, 0, 0, 0, 0, 2},
 { 7, 0, 0, 0, 0, 0, 0, 2, 0, 2, 0, 0, 0, 0, 0, 2},
 { 7, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 2},
 { 7, 0, 0, 0, 0, 0, 0, 2, 0, 2, 0, 0, 0, 0, 0, 2},
 { 7, 7, 7, 7, 7, 0, 0, 2, 0, 2, 0, 0, 0, 0, 0, 2},
 { 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2}
};

int image=13;

float viewing_angle=0;
int viewer_height=32;
int xview=8*64;
int yview=8*64;

void main(int argc,char* argv[])
{
  // Read arguments from command line if present:

	if (argc==2) viewing_angle=atof(argv[1]);

  // Point variable at video memory:

  char far *screen=(char far *)MK_FP(0xa000,0);

  // Save previous video mode:

	int oldmode=*(int *)MK_FP(0x40,0x49);

  // Set mode 13h:

  setmode(0x13);

  // Clear the screen:

  cls(screen);

  // Draw a raycast view of the maze:

  draw_maze(map,screen,xview,yview,viewing_angle,viewer_height);

  // Wait for user to hit a key:

  while (!kbhit());

  // Reset video mode and exit:

  setmode(oldmode);
}
