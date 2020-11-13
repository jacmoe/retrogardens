//
// POLYTEXT.CPP
//
// Function to draw a texture mapped polygon defined by
// polytype parameter POLY clipped against a viewport with
// upper lefthand coordinates of LEFTX,LEFTY and lower
// righthand coordinates of RIGHTX,RIGHTY.
//
// Written by Christopher Lampton for
// GARDENS OF IMAGINATION (Waite Group Press, 1994)

#include <stdio.h>
#include <conio.h>
#include <math.h>
#include "polytext.h"

void polytext(tpolytype *tpoly,int leftx,int lefty,int rightx,
			 int righty,char *screen)
{
	float tslope,bslope;
	int vwidth,vy,vheight;

  // Initialize pointer into texture map array:

	unsigned int tptr=0;

  // Initialize height and width of texture map:

	int theight=tpoly->htext;
	int twidth=tpoly->wtext;
	int swidth=tpoly->x[1]-tpoly->x[0]+1;

  // If polygon crosses left side of viewport, clip it:

	if (tpoly->x[0]<leftx) {

    // Calculate offset of clip in texture map array:

		int wtemp=tpoly->x[1]-tpoly->x[0]+1;
		int offtemp=leftx-tpoly->x[0];
		float xratio=(float)tpoly->wtext/wtemp;
		tptr=offtemp*xratio;
		tpoly->wtext-=tptr;

    // Calculate slopes for top and bottom edges:

		tslope=(float)(tpoly->y[1]-tpoly->y[0])/(tpoly->x[1]-tpoly->x[0]);
		bslope=(float)(tpoly->y[3]-tpoly->y[2])/(tpoly->x[0]-tpoly->x[1]);

    // Find new endpoints for clipped lines:

		tpoly->y[0]=tpoly->y[0]+tslope*(leftx-tpoly->x[0]);
		tpoly->y[3]=tpoly->y[3]+bslope*(leftx-tpoly->x[0]);
		tpoly->x[0]=leftx;
	}

  // Initialize x,y coordinates for polygon drawing:

	int x=tpoly->x[0];
	int y=tpoly->y[0];

  // Calculate the change in y coordinates for top
  // and bottom edges:

	int topdiff=tpoly->y[1]-tpoly->y[0];
	int botdiff=tpoly->y[2]-tpoly->y[3];

  // Initialize height and width of clipped polygon:

	int height=tpoly->y[3]-tpoly->y[0];
	int width=tpoly->x[1]-tpoly->x[0]+1;

  // Clip polygon width against right side of viewport:

	if (tpoly->x[1]>rightx)
		vwidth=width-(tpoly->x[1]-rightx);
	else vwidth=width;

    // Initialize top and bottom error terms:

	int toperror=0;
	int boterror=0;

  // Initialize horizontal error term for texture map:

	int txerror=0;

  // Loop across width of polygon:

	for (int w=0; w<vwidth; w++) {

    // Initialize temporary offset into texture map:

		int t=tptr;

    // If top of current vertical line is outside of
    // viewport, clip it:

		if (y<lefty) {
			vy=lefty;
			vheight=height-(lefty-y);
			float yratio=(float)tpoly->htext/height;
			t+=(lefty-y)*yratio;
		}
		else {
			vy=y;
			vheight=height;
		}
		if ((vy+vheight)>righty)
			vheight=righty-vy;

    // Point to video memory offset for top of line:

		unsigned int ptr=vy*320+x;

    // Initialize vertical error term for texture map:

		int tyerror=0;

    // Loop through the pixels in the current vertical
    // line, advancing PTR to the next row of pixels after
    // each pixel is drawn.

		for(int h=0; h<vheight; h++) {
			screen[ptr]=tpoly->textmap[t];

      // Is it time to move to next row of pixels in map?

			while (tyerror<theight) {
				tyerror+=height;
				t+=twidth;
			}
			tyerror-=theight;
			ptr+=320;
		}
		x++;

    // Advance x coordinate to next pixel column:

		toperror+=abs(topdiff);

      // If so move it up...

		while (toperror>=width) {

      // If so move it up...

			toperror-=width;
			if (topdiff>0) {
				y++;
				--height;
			}
			else {
				--y;
				height++;
			}
		}
		boterror+=abs(botdiff);
		while (boterror>=width) {
			boterror-=width;
			if (botdiff>0) height++;
			else --height;
		}

//  Is it time to move to next pixel column in map?

		while (txerror<twidth) {
			txerror+=swidth;
      tptr++;
		}
		txerror-=twidth;
  }
}
