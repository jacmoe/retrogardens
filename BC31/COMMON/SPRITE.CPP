
// SPRITE.CPP
//   Functions for sprite class
//   Includes basic operations for moving and erasing sprite images
//   with transparency
//
// Copyright 1993 by Christopher Lampton

#include	<stdio.h>
#include	<alloc.h>
#include	<mem.h>
#include	"sprite.h"
#include	"screen.h"

Sprite::Sprite(int num_sprites,int w,int h)

// Sprite constructor
// Allocates memory for NUM_SPRITES sprite structures

{
	image=new char far *[num_sprites];
	width=w;
	height=h;
	savebuffer=new char[width*height];
}

void Sprite::grab(unsigned char far *buffer,int sprite_num,int x1,int y1)
{

// "Grab" a rectangular sprite image from a 64K bitmap
//  and store in SPRITE[SPRITE_NUM]

	// Allocate memory for sprite bitmap and background save buffer:

	image[sprite_num]=new char[width*height];

	// Check if sprite runs past edge of bitmap buffer; abort if so:

	if ((x1+width>SCREEN_WIDTH) || (y1+height>SCREEN_WIDTH)) return;

	// Loop through rows and columns of sprite, storing pixels in sprite buffer:

	for(int row=0; row<height; row++)
		for(int column=0; column<width; column++)
			image[sprite_num][row*width+column]=buffer[(y1+row)*SCREEN_WIDTH+x1+column];

}

void Sprite::put(int sprite_num,int x,int y,unsigned char far *screen)

// Draw sprite on screen with upper lefthand corner at coordinates x,y
// Zero pixels are treated as transparent; i.e. they are not drawn.

{
	int poffset=y*320+x; // Calculate vidram offset of sprite coordinates
	int soffset=0;

	// Loop through rows of sprite, transferring nonzero pixels to screen:

	for(int row=0; row<height; row++) {
		for(int column=0; column<width; column++) {

			// Copy background pixel into background save buffer:

			savebuffer[soffset]=screen[poffset];

			// Get next pixel from sprite image buffer:

			int pixel=image[sprite_num][soffset++];

			// If not transparent, place in in the screen buffer:

			if (pixel) screen[poffset] = pixel;
			poffset++;
		}
		poffset+=(320-width);
	}

	// Record current coordinates of sprite:

	xsprite = x;
	ysprite = y;
}

void Sprite::erase(unsigned char far *screen)

// Erase sprite from screen by copying saved background image on top of it

{
	int voffset=ysprite*320+xsprite; // Calculate vidram offset of sprite coordinates
	int soffset=0;

	// Loop through rows and columns of background save buffer,
	// transferring pixels to screen buffer:

	for(int column=0; column<width; column++) {
		for(int row=0; row<height; row++)
			screen[voffset++]=savebuffer[soffset++];
		voffset+=(320-width); // Position pointer to next line
	}

}
