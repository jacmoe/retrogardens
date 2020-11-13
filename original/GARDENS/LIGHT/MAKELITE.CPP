//
// MAKELITE.CPP
//
// Utility for generating light-sourcing tables.
// Written by Christopher Lampton
// for Gardens of Imagination (1994)
//

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <values.h>
#include <conio.h>
#include <io.h>
#include <fcntl.h>
#include "pcx.h"

const MAXLIGHT=32;
const PALETTESIZE=256;

pcx_struct litepal;
int red[PALETTESIZE],green[PALETTESIZE],
    blue[PALETTESIZE];
unsigned char litesource[MAXLIGHT+1][PALETTESIZE];
float redscore,greenscore,bluescore;
FILE *handle;

void main(int argc,char* argv[])
{

  // Set default target to black:

  int redtarget=0;
  int greentarget=0;
  int bluetarget=0;

  // Check for filenames on the command line:

  if (argc<2) {
    printf("You must type a name for the source file.\n");
    exit(1);
  }

  if (argc<3) {
    printf("You must type a name for the target file.\n");
    exit(1);
  }

  // Check for target values on command line:

  if (argc>=4) redtarget=atof(argv[3]);
  if (argc>=5) bluetarget=atof(argv[4]);
  if (argc>=6) greentarget=atof(argv[5]);

  // Let user know what's going on:

  printf("Loading palette.\n");

  // Load the PCX file:

  if (loadPCX(argv[1],&litepal)) exit(1);

  // Copy palette to RGB arrays:

  for (int color=0; color<PALETTESIZE; color++) {
    red[color]=litepal.palette[color*3];
    green[color]=litepal.palette[color*3+1];
    blue[color]=litepal.palette[color*3+2];
  }

  // Let user know what we're about to do:

  printf("Calculating lightsourcing tables");

  // Iterate through all 256 palette colors:

  for (color=0; color<PALETTESIZE; color++) {

    // Let user know we haven't died:

    printf(".");

    // Iterate through all 32+1 intensity levels:

    for (int level=0; level<=MAXLIGHT; level++) {

      // Calculate ideal lightsourced RGB values:

      float redlite=(float)(red[color]-redtarget) / MAXLIGHT
              * level + redtarget;
      float greenlite=(float)(green[color]-greentarget) / MAXLIGHT
              * level + greentarget;
      float bluelite=(float)(blue[color]-bluetarget) / MAXLIGHT
              * level + bluetarget;

      // Initialize score to very large number:

      float bestscore=MAXFLOAT;

      // Search the palette for closest match:

      for (int color2=0; color2<PALETTESIZE; color2++) {

        // Assign proximity score to color:

        redscore=fabs(red[color2]-redlite);
        greenscore=fabs(green[color2]-greenlite);
        bluescore=fabs(blue[color2]-bluelite);
        float score=redscore+greenscore+bluescore;

        // Is this better (i.e. smaller) than best score?

        if (score<bestscore) {

          // If so, replace best score:

          bestscore=score;

          // And remember which color got it:

          litesource[level][color]=color2;
        }
      }
    }
  }

  // Tell user that the worst is over:

  printf("\nWriting lightsourcing tables to disk.\n");

  // Try to open disk file:

  if ((handle=fopen(argv[2],"wb"))==NULL) {
    perror("Error");
    exit;
  }

  // If successful, write lightsource tables to file:

  fwrite(litesource,MAXLIGHT+1,PALETTESIZE,handle);

  // Close up shop:

  fclose(handle);

  // And tell the user to breathe easy:

  printf("Done!\n");
}
