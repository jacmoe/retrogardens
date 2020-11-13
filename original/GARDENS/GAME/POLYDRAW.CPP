#include <stdio.h>
#include <math.h>
#include "polydraw.h"

// Function to draw a polygon defined by POLYTYPE parameter
// POLY in color COLOR clipped against a viewport with upper
// lefthand coordinates of LEFTX,LEFTY and lower righthand
// coordinates of RIGHTX,RIGHTY.

void polydraw(polytype *poly,int leftx,int lefty,int rightx,
			 int righty,int color,char *screen)
{
	float tslope,bslope;
	int wwidth,wy,wheight;

  // If polygon crosses left side of viewport, clip it:

	if (poly->x[0]<leftx) {

    // Calculate slopes for top and bottom edges:

		tslope=(float)(poly->y[1]-poly->y[0])/(poly->x[1]-poly->x[0]);
		bslope=(float)(poly->y[3]-poly->y[2])/(poly->x[0]-poly->x[1]);

    // Find new endpoints for clipped lines:

		poly->y[0]=poly->y[0]+tslope*(leftx-poly->x[0]);
		poly->y[3]=poly->y[3]+bslope*(leftx-poly->x[0]);
		poly->x[0]=leftx;
	}

  // Initialize x,y coordinates for polygon drawing:

	int x=poly->x[0];
	int y=poly->y[0];

  // Calculate the change in y coordinates for top
  // and bottom edges:

	int topdiff=poly->y[1]-poly->y[0];
	int botdiff=poly->y[2]-poly->y[3];

  // Initialize height and width of clipped polygon:

	int height=poly->y[3]-poly->y[0];
	int width=poly->x[1]-poly->x[0]+1;

  // Clip polygon width against right side of viewport:

	if (poly->x[1]>rightx)
		wwidth=width-(poly->x[1]-rightx);
	else wwidth=width;

  // Initialize top and bottom error terms:

	int toperror=0;
	int boterror=0;

  // Loop across width of polygon:

	for (int w=0; w<wwidth; w++) {

    // If top of current vertical line is outside of
    // viewport, clip it:

		if (y<lefty) {
			wy=lefty;
			wheight=height-(lefty-y);
		}
		else {
			wy=y;
			wheight=height;
		}
		if ((wy+wheight)>righty)
			wheight=righty-wy;

    // Point to video memory offset for top of line:

		unsigned int ptr=wy*320+x;

    // Loop through the pixels in the current vertical
    // line, advancing PTR to the next row of pixels after
    // each pixel is drawn.

		for(int h=0; h<wheight; h++) {
			screen[ptr]=color;
			ptr+=320;
		}

    // Advance x coordinate to next vertical line:

		x++;

    // Is it time to move the top edge up or down?

		toperror+=abs(topdiff);

		while (toperror>=width) {

      // If so move it up...

			toperror-=width;
			if (topdiff>0) {
				y++;
				--height;
			}

      // or down.

			else {
				--y;
				height++;
			}
		}

    // Is it time to move the bottom edge up or down?

		boterror+=abs(botdiff);
		while (boterror>=width) {

      // If so, move it:

			boterror-=width;
			if (botdiff>0) height++;
			else --height;
		}
	}
}
