#include <stdio.h>
#include <dos.h>
#include <conio.h>
#include <stdlib.h>
#include <mem.h>
#include <math.h>
#include "screen.h"
#include "pcx.h"


const WALL_HEIGHT = 64;
const VIEWER_DISTANCE = 192;
const VIEWPORT_LEFT = 0;
const VIEWPORT_RIGHT = 319;
const VIEWPORT_TOP = 0;
const VIEWPORT_BOT = 199;
const VIEWPORT_HEIGHT = VIEWPORT_BOT - VIEWPORT_TOP;
const VIEWPORT_CENTER = VIEWPORT_TOP + VIEWPORT_HEIGHT / 2;

const IMAGE_HEIGHT = 64;
const IMAGE_WIDTH = 64;

typedef int map_type[16][16];

map_type map =
{
  { 5, 5, 5, 5,   5, 5, 5, 5,   5, 5, 5, 5,   5, 5, 5, 5},
  { 7, 5, 0, 5,   0, 0, 0, 2,   0, 0, 0, 0,   0, 0, 0, 5},
  { 7, 0, 0, 0,   0, 0, 0, 0,   3, 0, 0, 0,   0, 0, 0, 5},
  { 7, 0, 0, 0,   0, 0, 0, 0,   0, 6, 0, 0,   0, 0, 0, 5},

  { 7, 0, 0, 0,   0, 0, 0, 0,   0, 0, 5, 4,   0, 0, 0, 5},
  { 8, 0, 0, 0,   0, 0, 0, 0,   0, 0, 6, 6,   0, 0, 0, 5},
  { 6, 0, 0, 0,   0, 0, 0, 0,   0, 0, 0, 6,   8, 0, 0, 5},
  { 6, 0, 0, 0,   0, 0, 0, 0,   0, 0, 0, 0,   8, 0, 0, 5},

  { 6,10, 8, 0,   0, 0, 0, 0,   8, 0, 0, 6,   8, 0, 0, 5},
  { 8, 0, 0, 0,   0, 0, 0, 0,   0, 0, 6, 6,   0, 0, 0, 5},
  { 8, 7, 7, 0,   0, 0, 5, 1,   0, 1, 0, 0,   0, 0, 0, 5},
  { 6, 0, 0, 0,   0, 0, 0, 1,   0, 1, 0, 0,   0, 0, 0, 5},

  { 7, 0, 0, 0,   0, 0, 0, 1,   0, 0, 0, 0,   0, 0, 0, 8},
  { 7, 0, 0, 0,   0, 0, 0, 1,   0, 1, 0, 0,   0, 0, 0, 6},
  { 7, 6, 6, 6,   6, 0, 0, 1,   0, 1, 0, 0,   0, 0, 0, 8},
  { 7, 5, 5, 5,   5, 5, 5, 8,   8, 8, 5, 5,   5, 5, 5, 5}
};

map_type flor =
{
  { 1, 1, 1, 1,   1, 1, 1, 1,   1, 1, 1, 1,   1, 1, 1, 1},
  { 1, 1, 1, 1,   1, 1, 1, 1,   1, 1, 1, 1,   1, 1, 1, 1},
  { 1, 1, 1, 1,   1, 1, 1, 1,   1, 1, 1, 1,   1, 1, 1, 1},
  { 1, 1, 1, 1,   1, 1, 1, 1,   1, 1, 1, 1,   1, 1, 1, 1},

  { 1, 1, 1, 1,   1, 1, 1, 1,   1, 1, 1, 1,   1, 1, 1, 1},
  { 1, 1, 1, 1,   1, 1, 1, 1,   1, 1, 1, 1,   1, 1, 1, 1},
  { 1, 1, 1, 1,   1, 1, 1, 1,   1, 1, 1, 1,   1, 1, 1, 1},
  { 1, 1, 1, 1,   1, 1, 1, 1,   1, 1, 1, 1,   1, 1, 1, 1},

  { 1, 1, 1, 1,   1, 1, 1, 1,   1, 1, 1, 1,   1, 1, 1, 1},
  { 1, 1, 1, 1,   1, 1, 1, 1,   1, 1, 1, 1,   1, 1, 1, 1},
  { 1, 1, 1, 1,   1, 1, 1, 1,   1, 1, 1, 1,   1, 1, 1, 1},
  { 1, 1, 1, 1,   1, 1, 1, 1,   1, 1, 1, 1,   1, 1, 1, 1},

  { 1, 1, 1, 1,   1, 1, 1, 1,   1, 1, 1, 1,   1, 1, 1, 1},
  { 1, 1, 1, 1,   1, 1, 1, 1,   1, 1, 1, 1,   1, 1, 1, 1},
  { 1, 1, 1, 1,   1, 1, 1, 1,   1, 1, 1, 1,   1, 1, 1, 1},
  { 1, 1, 1, 1,   1, 1, 1, 1,   1, 1, 1, 1,   1, 1, 1, 1}
};

map_type ceiling =
{
  { 5, 5, 5, 5,   5, 5, 5, 5,   5, 5, 5, 5,   5, 5, 5, 5},
  { 5, 5, 5, 5,   5, 5, 5, 5,   5, 5, 5, 5,   5, 5, 5, 5},
  { 5, 5, 5, 5,   5, 5, 5, 5,   5, 5, 5, 5,   5, 5, 5, 5},
  { 5, 5, 5, 5,   5, 5, 5, 5,   5, 5, 5, 5,   5, 5, 5, 5},

  { 5, 5, 5, 5,   5, 5, 5, 5,   5, 5, 5, 5,   5, 5, 5, 5},
  { 5, 5, 5, 5,   5, 5, 5, 5,   5, 5, 5, 5,   5, 5, 5, 5},
  { 5, 5, 5, 5,   5, 5, 5, 5,   5, 5, 5, 5,   5, 5, 5, 5},
  { 5, 5, 5, 5,   5, 5, 5, 5,   5, 5, 5, 5,   5, 5, 5, 5},

  { 5, 5, 5, 5,   5, 5, 5, 5,   5, 5, 5, 5,   5, 5, 5, 5},
  { 5, 5, 5, 5,   5, 5, 5, 5,   5, 5, 5, 5,   5, 5, 5, 5},
  { 5, 5, 5, 5,   5, 5, 5, 5,   5, 5, 5, 5,   5, 5, 5, 5},
  { 5, 5, 5, 5,   5, 5, 5, 5,   5, 5, 5, 5,   5, 5, 5, 5},

  { 5, 5, 5, 5,   5, 5, 5, 5,   5, 5, 5, 5,   5, 5, 5, 5},
  { 5, 5, 5, 5,   5, 5, 5, 5,   5, 5, 5, 5,   5, 5, 5, 5},
  { 5, 5, 5, 5,   5, 5, 5, 5,   5, 5, 5, 5,   5, 5, 5, 5},
  { 5, 5, 5, 5,   5, 5, 5, 5,   5, 5, 5, 5,   5, 5, 5, 5}
};
void draw_maze(map_type map, map_type flor, map_type ceiling,
				char far* screen, int xview, int yview,
				float viewing_angle, int viewer_height,
				char far* textmaps);

void main(int argc, char* argv[])
{
	float viewing_angle = 3;
	int viewer_height = 32;
	int xview = 8 * 64;
	int yview = 8 * 64;

	// read arguments from command line if present:
	if(argc == 2) viewing_angle = atof(argv[1]);

	// Put display in mode 13h
	set_mode(VGA_256_COLOR_MODE);

	byte* double_buffer;

	if((double_buffer = (byte*) malloc(SCREEN_SIZE)) == NULL)
	{
		printf("Not enough memory for double buffer.\n");
		exit(1);
	}

	PcxImage pcx;
	//if(PcxLoadImage("tex\\aztec1.pcx", &pcx) != PCX_OK) exit(1);
	if(PcxLoadImage("images.pcx", &pcx) != PCX_OK) exit(1);
	set_palette(pcx.pal);

	// clear double buffer
	memset(double_buffer, 0, SCREEN_SIZE);


	draw_maze(map, flor, ceiling, double_buffer, xview, yview, viewing_angle,
				viewer_height, pcx.bitmap);

	// show the buffer
	show_buffer(double_buffer);

	while(!kbhit());

	free(double_buffer);

	// enter text mode
	set_mode(TEXT_MODE);
}

// Draws a raycast image in the viewport of the maze represented in array
// map as seen from position xview, yview by a viewer looking at an angle
// viewer_angle, where angle 0 is due north. (Angles measured in radians)
void draw_maze(map_type map, map_type flor, map_type ceiling,
				char far* screen, int xview, int yview,
				float viewing_angle, int viewer_height,
				char far* textmaps)
{
	int sy, offset;				// pixel y position and offset
	float xd, yd;				// distance to next wall in x and y
	int grid_x, grid_y;			// coordinates of x and y grid lines
	float xcross_x, xcross_y;	// ray intersection coordinates
	float ycross_x, ycross_y;
	unsigned int xdist, ydist;	// distance to x and y grid lines
	int xmaze, ymaze;			// map location of ray collision
	int distance;				// distance to wall along ray
	int tmcolumn;				// column in texture map
	float yratio;

	// *** The raycasting begins:

	// loop through all columns of pixels in viewport:
	for(int column = VIEWPORT_LEFT; column < VIEWPORT_RIGHT; column++)
	{
		// calculate horizontal angle of ray relative to center ray:
		float column_angle = atan((float)(column - 160) / VIEWER_DISTANCE);

		// calculate angle of ray relative to maze coordinates:
		float radians = viewing_angle + column_angle;

		// rotate endpoint of ray to viewing angle:
		int x2 = -1024 * (sin(radians));
		int y2 = 1024 * (cos(radians));

		// translate relative to viewer's position:
		x2 += xview;
		y2 += yview;

		// initialize ray at viewer's position:
		float x = xview;
		float y = yview;

		// find difference in x,y coordates along the ray:
		int xdiff = x2 - xview;
		int ydiff = y2 - yview;

		// cheat to avoid divide-by-zero error:
		if(xdiff == 0) xdiff = 1;

		// get slope of ray:
		float slope = (float)ydiff / xdiff;

		// cheat (again) to avoid divide-by-zero error:
		if(slope == 0.0) slope = 0.0001;

		// cast ray from grid line to grid line:
		for(;;)
		{
			// if ray direction positive in x, get next x grid line:
			if(xdiff > 0) grid_x = ((int)x & 0xffc0) + 64;

			// if ray direction negative in x, get last x grid line:
			else grid_x = ((int)x & 0xffc0) - 1;

			// if ray direction positive in y, get next y grid line:
			if(ydiff > 0) grid_y = ((int)y & 0xffc0) + 64;

			// if ray direction negative in y, get last y grid line:
			else grid_y = ((int)y & 0xffc0) - 1;

			// get y,y coordinates where ray crosses x grid line:
			xcross_x = grid_x;
			xcross_y = y + slope * (grid_x - x);

			// get x,y coordinates where ray crosses y grid line:
			ycross_x = x + (grid_y - y) / slope;
			ycross_y = grid_y;

			// get distance to x grid line:
			xd = xcross_x - x;
			yd = xcross_y - y;
			xdist = sqrt(xd * xd + yd * yd);

			// get distance to y grid line:
			xd = ycross_x - x;
			yd = ycross_y - y;
			ydist = sqrt(xd * xd + yd * yd);

			// if x grid line is closer ...
			if(xdist < ydist)
			{
				// calculate maze grid coordinates of square:
				xmaze = xcross_x / 64;
				ymaze = xcross_y / 64;

				// set x and y to point of ray intersection:
				x = xcross_x;
				y = xcross_y;

				// find relevant column of texture map:
				tmcolumn = (int)y &0x3f;

				// is there a maze cube here? If so, stop looping:
				if(map[xmaze][ymaze]) break;
			}
			else	// if y grid line is closer:
			{
				// calculate maze grid coordinates of square:
				xmaze = ycross_x / 64;
				ymaze = ycross_y / 64;

				// set x and y to piont of ray intersection:
				x = ycross_x;
				y = ycross_y;

				// find relevant column of texture map:
				tmcolumn = (int)x &0x3f;

				// is there a maze cube here? If so, stop looping:
				if(map[xmaze][ymaze]) break;
			}

		} // raycast loop

		// *** Prepare to draw wall column:

		// get distance from viewer to intersection point:
		xd = x - xview;
		yd = y - yview;
		distance = (long)sqrt(xd * xd + yd * yd) * cos(column_angle);
		if(distance == 0) distance = 1;

		// calculate visible height of walls:
		int height = VIEWER_DISTANCE * WALL_HEIGHT / distance;

		// calculate bottom of wall on screen:
		int bot = VIEWER_DISTANCE * viewer_height / distance + VIEWPORT_CENTER;

		// calculate top of wall on screen:
		int top = bot - height + 1;

		// initialize temporary offset into texture map:
		int t = tmcolumn;

		// if top of current vertical line is outside of viewport, clip it:
		int dheight = height;
		int iheight = IMAGE_HEIGHT;
		yratio = (float)WALL_HEIGHT / height;
		if(top < VIEWPORT_TOP)
		{
			dheight -= (VIEWPORT_TOP - top);
			t += (int)((VIEWPORT_TOP - top) * yratio) * 256;// 320?
			iheight -= ((VIEWPORT_TOP - top) * yratio);
			top = VIEWPORT_TOP;
		}
		if(bot > VIEWPORT_BOT)
		{
			dheight -= (bot - VIEWPORT_BOT);
			iheight -= (bot - VIEWPORT_BOT) * yratio;
			bot = VIEWPORT_BOT;
		}

		// *** Draw the wall column:

		// point to video memory offset for top of line:
		offset = top * 320 + column;

		// initialize vertical error term for texture map:
		int tyerror = 64;

		// which graphics tile are we using?
		int tile = map[xmaze][ymaze] - 1;

		// find offset of tile and column in bitmap:
		unsigned int tileptr = (tile / 4) * 256 * IMAGE_HEIGHT + (tile % 4) * IMAGE_WIDTH + t;

		// loop through all pixels in the current vertical line,
		// advancing offset to the next row of pixels after each pixel is drawn
		for(int h = 0; h < iheight; h++)
		{
			// are we ready to draw a pixel?
			while(tyerror >= IMAGE_HEIGHT)
			{
				// if so, draw it:
				screen[offset] = textmaps[tileptr];

				// reset error term:
				tyerror -= IMAGE_HEIGHT;

				// and advance offset to next screen line:
				offset += 320;
			}
			// incremental division:
			tyerror += height;

			// advance tileptr to next line of bitmap:
			tileptr += 256;// 320?
		}
		///*
		// step through floor pixels
		for(int row = bot + 1; row <= VIEWPORT_BOT; row++)
		{
			// get ratio of viewer's height to pixel height:
			float ratio = (float)viewer_height / (row - 100);

			// get distance to visible pixel:
			distance = ratio * VIEWER_DISTANCE / cos(column_angle);

			// rotate distance to ray angle:
			int x = - distance * (sin(radians));
			int y = distance * (cos(radians));

			// translate relative to viewer coordinates:
			x += xview;
			y += yview;

			// get maze square intersected by ray:
			int xmaze = x / 64;
			int ymaze = y / 64;

			// find relevant column of texture map:
			int t = ((int)y & 0x3f) * 256 + ((int)x & 0x3f);

			// which graphics tile are we using?
			int tile = flor[xmaze][ymaze] - 1;

			// find offset of tile and column in bitmap:
			unsigned int tileptr = (tile/4) * 256 * IMAGE_HEIGHT + (tile % 4) * IMAGE_WIDTH + t;

			// calculate video offset of floor pixel:
			offset = row * 320 + column;

			// draw pixel:
			screen[offset] = textmaps[tileptr];
		}
		//*/
		///*
		// step through ceiling pixels
		for(row = top - 1; row >= VIEWPORT_TOP; --row)
		{
			// get ratio of viewer's height to pixel height:
			float ratio = (float)(WALL_HEIGHT - viewer_height) / (100 - row);

			// get distance to visible pixel:
			distance = ratio * VIEWER_DISTANCE / cos(column_angle);

			// rotate distance to ray angle:
			int x = - distance * (sin(radians));
			int y = distance * (cos(radians));

			// translate relative to viewer coordinates:
			x += xview;
			y += yview;

			// get maze square intersected by ray:
			int xmaze = x / 64;
			int ymaze = y / 64;

			// find relevant column of texture map:
			int t = ((int)y & 0x3f) * 256 + ((int)x & 0x3f);

			// which graphics tile are we using?
			int tile = ceiling[xmaze][ymaze] - 1;

			// find offset of tile and column in bitmap:
			unsigned int tileptr = (tile/4) * 256 * IMAGE_HEIGHT + (tile % 4) * IMAGE_WIDTH + t;

			// calculate video offset of ceiling pixel:
			offset = row * 320 + column;

			// draw pixel:
			screen[offset] = textmaps[tileptr];

		}
		//*/
	} // for each column in viewport

}

