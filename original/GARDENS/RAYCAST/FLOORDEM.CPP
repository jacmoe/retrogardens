
// FLOORDEM.CPP
//
// Calls raycasting function to draw texture-mapped
// view of maze floor.
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
#include  "floorcst.h"

const NUM_IMAGES=15;

pcx_struct textmaps;

map_type flor={
 { 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5},
 { 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5},
 { 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5},
 { 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5},
 { 5, 5, 5, 5, 0, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5},
 { 5, 5, 5, 0, 0, 0, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5},
 { 5, 5, 0, 0, 0, 0, 0, 5, 5, 5, 5, 5, 5, 5, 5, 5},
 { 5, 5, 5, 0, 0, 0, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5},
 { 5, 5, 5, 5, 0, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5},
 { 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5},
 { 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5},
 { 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5},
 { 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5},
 { 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5},
 { 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5},
 { 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5}
};

map_type ceiling={
 {13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13},
 {13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13},
 {13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13},
 {13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13},
 {13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13},
 {13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13},
 {13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13},
 {13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13},
 {13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13},
 {13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13},
 {13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13},
 {13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13},
 {13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13},
 {13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13},
 {13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13},
 {13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13}
};

float viewing_angle=0;
int viewer_height=32;
int xview=8*64;
int yview=8*64;

void main(int argc,char* argv[])
{
  // Read arguments from command line if present:

	if (argc==2) viewing_angle=atof(argv[1]);

  // Load texture map images:

	if (loadPCX("walls.pcx",&textmaps)) exit(1);

  // Point variable at video memory:

  char far *screen=(char far *)MK_FP(0xa000,0);

  // Save previous video mode:

	int oldmode=*(int *)MK_FP(0x40,0x49);

  // Set mode 13h:

  setmode(0x13);

  // Set the palette:

  setpalette(textmaps.palette);

  // Clear the screen:

  cls(screen);

  // Draw a raycast view of the maze:

  draw_maze(flor,ceiling,screen,xview,yview,viewing_angle,
              viewer_height,textmaps.image);

  // Wait for user to hit a key:

  while (!kbhit());

  // Release memory:

  delete textmaps.image;

  // Reset video mode and exit:

  setmode(oldmode);
}
