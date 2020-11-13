
// PCXSHOW.CPP Version 1.0
// Display a PCX image on the mode 13h screen
//
// Written on July 29, 1993 by Christopher Lampton
//   for GARDENS OF IMAGINATION (Waite Group Press)

#include  <stdio.h>
#include	<dos.h>
#include	<conio.h>
#include  <stdlib.h>
#include  "screen.h"
#include  "pcx.h"

pcx_struct pcx;

void main(int argc,char *argv[])
{
  if (argc!=2) { // Are there 2 arguments on command line?

    puts("Wrong number of arguments.\n");
    exit(-1);

  }

  // Create pointer to video memory:

  char far *screen=(char far *)MK_FP(0xa000,0);

  // Save previous video mode:

  int oldmode=*(int *)MK_FP(0x40,0x49);

  // Put display in mode 13h:

  setmode(0x13);

  // Clear display:

  cls(screen);

    if (loadPCX(argv[1],&pcx)) {
      setmode(oldmode);
      exit(1);
    }
  int image_width=pcx.header.xmax - pcx.header.xmin + 1;
  int image_height=pcx.header.ymax - pcx.header.ymin + 1;
  long image_size=(long)image_width * image_height;
  setpalette(pcx.palette);
  for (int y=0; y<image_height; y++) {
    for (int x=0; x<image_width; x++) {
      screen[y*320+x]=pcx.image[y*image_width+x];
    }
  }

  while (!kbhit());

  // Release memory:

  delete pcx.image;

  // Restore old video mode:

  setmode(oldmode);
}
