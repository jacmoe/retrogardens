
// TARGA.CPP
//  A function for loading and compressing Targa images.
//
// Written August 11, 1993 by Christopher Lampton
// for GARDENS OF IMAGINATION (Waite Group Press)

#include	<stdio.h>
#include	<dos.h>
#include  <io.h>
#include	<fcntl.h>
#include	<mem.h>
#include "targa.h"

static int infile; // Handle for Targa file

int loadTGA(char far *filename,tga_struct *tga)
// Loads an 8-bit uncompressed Targa file into a structure of type tga_struct
{

  // Open file, abort if not found:

  if ((infile=open(filename,O_BINARY))==-1) return(-1);

  // Set file pointer at start of file:

  lseek(infile,0L,SEEK_SET);

  // Read header into header field:

  read(infile,&(tga->header),sizeof(tga_header));

  // Is this an uncompressed color-mapped image? If not, abort:

  if (tga->header.image_type != 1) return(-1);

  // Is the image wider than 320 pixels? If so, abort:

  if (tga->header.image_width> 320) return(-1);

  // Is the image taller than 200 pixels? If so, abort:

  if (tga->header.image_height > 200) return(-1);

  // Is it an 8-bit image? If not, abort:

  if (tga->header.bits_per_pixel != 8) return(-1);

  // Does it have a 24-bit palette? If not, abort:

  if (tga->header.color_map_entry_size != 24) return(-1);

  // If there's an ID string, load it:

  if (tga->header.ID_length>0) {
    tga->ID=new char[tga->header.ID_length];
    read(infile,tga->ID,tga->header.ID_length);
  }
  else tga->ID = 0;

  // Allocate memory for palette, abort if not available:

  if ((tga->color_map=new unsigned char[3*256])==NULL) {
    if (tga->ID) delete tga->ID;
    return(-1);
  }

  // Load palette, translate for VGA:

  for (int entry=0; entry<256; entry++) {
    for (int color=2; color>=0; --color) {
      read(infile,&(tga->color_map[entry*3+color]),1);
      tga->color_map[entry*3+color] >>= 2;
    }
  }

  // Calculate size of image in bytes:

  unsigned int image_size=tga->header.image_width
    * tga->header.image_height;

  // Allocate memory for image, abort if not available:

  if ((tga->image=new unsigned char[image_size])==NULL) {
    delete tga->color_map;
	if (tga->ID) delete tga->ID;
    return(-1);
  }

  // Load image from file:

  read(infile,tga->image,image_size);

  // Close file:

  close(infile);

  // Return without error:

  return(0);
}

