
// BITSCALE.CPP Version 1.0
// Scales a bitmap on the mode 13h display
//
// Written by Christopher Lampton, September 15, 1993
//   for GARDENS OF IMAGINATION (Waite Group Press)

#include  <stdio.h>
#include	<dos.h>
#include	<conio.h>
#include  <stdlib.h>
#include  <time.h>
#include  "screen.h"
#include  "targa.h"
#include  "bitmap.h"

#define   FRAMES_PER_SECOND 2

const IMAGE_X=25;
const IMAGE_Y=21;
const IMAGE_WIDTH=76;
const IMAGE_HEIGHT=96;
const SCREENX=20;
const SCREENY=0;

long ticks_per_frame; // Number of clock ticks per
											// frame of animation

tga_struct mona,bg;
char bitmap[IMAGE_WIDTH*IMAGE_HEIGHT];

void main()
{

  // Create pointer to video memory:

  char far *screen=(char far *)MK_FP(0xa000,0);

  // Save previous video mode:

	int oldmode=*(int *)MK_FP(0x40,0x49);

	// Create offscreen video buffer:

	char far *screen_buffer=new unsigned char [64000];

  // Put display in mode 13h:

	setmode(0x13);

	// Set number of ticks per frame:

	ticks_per_frame = CLK_TCK / FRAMES_PER_SECOND;

	// Initialize the frame timer:

	clock_t lastframe=clock();

  // Load bitmap from TGA file:

  if (loadTGA("mona.tga",&mona)) {
     setmode(oldmode);
     exit(1);
  }

  if (loadTGA("monabg.tga",&bg)) {
     setmode(oldmode);
     exit(1);
  }

  // Set palette to TGA palette:

  setpalette(mona.color_map);

  // Keep looping until somebody hits a key:

  while (!kbhit()) {

    // Copy background image to screen buffer:

    for (unsigned int i=0; i<64000; i++)
      screen_buffer[i]=bg.image[i];

    // Expand picture from 10 to 200 percent:

    for (int percent=10; percent<=200; percent+=5) {

	  	// Pause until time for next frame:

		  while ((clock()-lastframe)<ticks_per_frame) ;

  		// Start timing another frame:

	  	lastframe=clock();

      // Initialize vertical error terms:

      int yerror=0;

      int screeny=(200-percent)/2;

      // Offset of first screen pixel in row:

      unsigned int scrptr=screeny*320+SCREENX;

      // Offset of first bitmap pixel in row:

      unsigned int bitptr=0;

      // Loop through rows of bitmap:

      for (int y=0; y<IMAGE_HEIGHT; y++) {

        // Should we draw next row?

        yerror+=percent;

        // If so, repeat row until done:

        while (yerror>100) {

          // Reset error term:

          yerror-=100;

          // Save screen row offset:

          unsigned int old_scrptr=scrptr;

          // Save bitmap row offset:

          unsigned int old_bitptr=bitptr;

          // Initialize horizontal error term:

          int xerror=0;

          // Loop through pixels in row:

          for (int x=0; x<IMAGE_WIDTH; x++) {

            // Should we draw next pixel?

            xerror+=percent;

            // If so, loop through all rows to be drawn:

            while (xerror>100) {

              // Reset error term:

              xerror-=100;

              // Copy next pixel from bitmap to screen:

              screen_buffer[scrptr]=mona.image[bitptr];

              // Point to next pixel on screen:

              scrptr++;
            }
            // Point to next pixel in bitmap:

            bitptr++;
          }

          // Point to next row on screen:

          scrptr=old_scrptr+320;

          // Repeat same row in bitmap:

          bitptr=old_bitptr;
        }

        // Point to next row in bitmap:

        bitptr+=IMAGE_WIDTH;
      }

  		// Move screen buffer to screen:

	  	blitscreen(screen_buffer);
    }
  }

  while (!kbhit());

  // Release memory

  delete bg.image;
  delete bg.color_map;
  if (bg.ID) delete bg.ID;
  delete mona.image;
  delete mona.color_map;
  if (mona.ID) delete mona.ID;
  delete screen_buffer;

  // Restore old video mode:

  setmode(oldmode);
}
