
// BLOKDEMO.CPP
//
// Calls raycasting function to draw view of
// block-aligned height fields.
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
#include  "blokcast.h"
//#include  "bitmap.h"

const NUM_IMAGES=15;

pcx_struct textmaps,highmaps;

map_type wall={
 { 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7},
 { 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7},
 { 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7},
 { 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7},
 { 1, 1, 1, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7},
 { 1, 1, 1, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7},
 { 1, 1, 1, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7},
 { 1, 1, 1, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7},
 { 1, 1, 1, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7},
 { 1, 1, 1, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7},
 { 1, 1, 1, 7, 2, 2, 2, 2, 2, 7, 7, 7, 7, 7, 7, 7},
 { 7, 7, 7, 7, 2, 2, 2, 2, 2, 7, 7, 7, 7, 7, 7, 7},
 { 7, 7, 7, 7, 2, 2, 2, 2, 2, 7, 7, 7, 7, 7, 7, 7},
 { 7, 7, 7, 7, 2, 2, 2, 2, 2, 7, 7, 7, 7, 7, 7, 7},
 { 7, 7, 7, 7, 2, 2, 2, 2, 2, 7, 7, 7, 7, 7, 7, 7},
 { 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7}
};

map_type flor={
 { 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2},
 { 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2},
 { 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2},
 { 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2},
 { 2, 2, 2, 2, 8, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2},
 { 2,10,10, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0},
 { 2,10,10, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0},
 { 2,10,10, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0},
 { 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0},
 { 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2},
 { 2, 2, 2, 2, 5, 5, 5, 5, 5, 2, 2, 2, 2, 2, 2, 2},
 { 2, 2, 2, 2, 5, 5, 5, 5, 5, 2, 2, 2, 2, 2, 2, 2},
 { 2, 2, 2, 2, 5, 5, 5, 5, 5, 2, 2, 2, 2, 2, 2, 2},
 { 2, 2, 2, 2, 5, 5, 5, 5, 5, 2, 2, 2, 2, 2, 2, 2},
 { 2, 2, 2, 2, 5, 5, 5, 5, 5, 2, 2, 2, 2, 2, 2, 2},
 { 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2}
};

map_type floorheight={
 {20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20},
 {20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20},
 {20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20},
 {20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20},
 {20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20},
 {20, 0, 0,20,20,20,20,20,20,20,40,60,80,100,120,120},
 {20, 0, 0,20,20,20,20,20,20,20,20,20,20,20,120,120},
 {20, 0, 0,20,20,20,20,20,20,20,20,20,20,20,120,120},
 {20,20,20,20,20,20,20,20,20,20,40,60,80,100,120,120},
 {20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20},
 {20,20,20,20,60,60,60,60,60,20,20,20,20,20,20,20},
 {20,20,20,20,60,100,100,100,60,20,20,20,20,40,20,20},
 {20,20,20,20,60,100,150,100,60,20,20,20,40,40,20,20},
 {20,20,20,20,60,100,100,100,60,20,20,40,40,40,20,20},
 {20,20,20,20,60,60,60,60,60,20,20,20,20,20,20,20},
 {20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20}
};

float viewing_angle=0;
int viewer_height=32;
int xview=6*64+48;
int yview=6*64;

void main(int argc,char* argv[])
{
  // Read arguments from command line if present:

	if (argc>=2) viewing_angle=atof(argv[1]);
	if (argc>=3) viewer_height=atof(argv[2]);

  // Load texture map images:

	if (loadPCX("..\\raycast\\walls.pcx",&textmaps)) exit(1);
	if (loadPCX("highmaps.pcx",&highmaps)) exit(1);

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

  draw_maze(wall,flor,floorheight,screen,
              xview,yview,viewing_angle,viewer_height,
              textmaps.image);

  // Wait for user to hit a key:

  while (!kbhit());

  // Release memory:

  delete highmaps.image;
  delete textmaps.image;

  // Reset video mode and exit:

  setmode(oldmode);
}
