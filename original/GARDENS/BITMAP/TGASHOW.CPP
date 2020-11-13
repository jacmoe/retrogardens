
// TGASHOW.CPP Version 1.0
// Display a 256-color Targa image on the mode 13h screen
//
// Written by Christopher Lampton 6:01 a.m., July 25, 1993
//   for GARDENS OF IMAGINATION (Waite Group Press)

#include  <stdio.h>
#include	<dos.h>
#include	<conio.h>
#include  <stdlib.h>
#include  "screen.h"
#include  "targa.h"

tga_struct tga;

void main(int argc,char *argv[])
{
  if (argc!=2) { // Are there 2 arguments on command line?
    puts("Wrong number of arguments.\n");
    exit(-1);
  }

  // Load Targa file. Abort if not found:

  if (loadTGA(argv[1],&tga)) exit(1);

  // Create pointer to video memory:

  char far *screen=(char far *)MK_FP(0xa000,0);

  // Save previous video mode:

  int oldmode=*(int *)MK_FP(0x40,0x49);

  // Put display in mode 13h:

  setmode(0x13);

  // Clear display:

  cls(screen);

  // Set palette to Targa color map:

  setpalette(tga.color_map);

  // Copy image to mode 13h display:

  for (int y=0; y<tga.header.image_height; y++) {
    for (int x=0; x<tga.header.image_width; x++) {
      screen[y*320+x]=tga.image[y*tga.header.image_width+x];
    }
  }

  // Pause until user presses a key:

  while (!kbhit());

  // Release memory:

  delete tga.image;
  delete tga.color_map;
  if (tga.ID) delete tga.ID;

  // Restore old video mode:

  setmode(oldmode);
}

