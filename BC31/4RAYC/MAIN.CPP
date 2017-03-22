#include <stdio.h>
#include <dos.h>
#include <conio.h>
#include <stdlib.h>
#include <mem.h>
#include <math.h>
#include "screen.h"


const WALL_HEIGHT = 64;
const VIEWER_HEIGHT = 32;
const VIEWER_DISTANCE = 128;
const VIEWPORT_LEFT = 40;
const VIEWPORT_RIGHT = 280;
const VIEWPORT_TOP = 50;
const VIEWPORT_BOT = 150;
const VIEWPORT_HEIGHT = 100;
const VIEWPORT_CENTER = 100;


typedef int map_type[16][16];

map_type map =
{
  { 5, 5, 5, 5,   5, 5, 5, 5,   5, 5, 5, 5,   5, 5, 5, 5},
  { 7, 5, 0, 5,   0, 0, 0, 2,   0, 0, 0, 0,   0, 0, 0, 5},
  { 7, 0, 0, 0,   0, 0, 0, 0,   3, 0, 0, 0,   0, 0, 0, 5},
  { 7, 0, 0, 0,   0, 0, 0, 0,   0, 6, 0, 0,   0, 0, 0, 5},

  { 7, 0, 0, 0,   0, 0, 0, 0,   0, 0, 5, 4,   0, 0, 0, 5},
  {12, 0, 0, 0,   0, 0, 0, 0,   0, 0,11,11,   0, 0, 0, 5},
  {11, 0, 0, 0,   0, 0, 0, 0,   0, 0, 0,11,  10, 0, 0, 5},
  {11, 0, 0, 0,   0, 0, 0, 0,   0, 0, 0, 0,  10, 0, 0, 5},

  {11, 7, 7, 0,   0, 0, 0, 0,   0, 0, 0,11,  10, 0, 0, 5},
  {12, 0, 0, 0,   0, 0, 0, 0,   0, 0,11,11,   0, 0, 0, 5},
  {12, 7, 7, 0,   0, 0, 5, 1,   0, 1, 0, 0,   0, 0, 0, 5},
  { 7, 0, 0, 0,   0, 0, 0, 1,   0, 1, 0, 0,   0, 0, 0, 5},

  { 7, 0, 0, 0,   0, 0, 0, 1,   0, 0, 0, 0,   0, 0, 0,12},
  { 7, 0, 0, 0,   0, 0, 0, 1,   0, 1, 0, 0,   0, 0, 0,11},
  { 7, 6, 6, 6,   6, 0, 0, 1,   0, 1, 0, 0,   0, 0, 0,10},
  { 7, 5, 5, 5,   5, 5, 5,15,  15,15, 5, 5,   5, 5, 5, 5}
};

void draw_maze(map_type map, char far* screen, int xview, int yview, float viewing_angle);


float viewing_angle = 62;
int viewer_height = 32;
int xview = 8 * 64;
int yview = 8 * 64;


void main(int argc, char* argv[])
{
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

	// clear double buffer
	memset(double_buffer, 0, SCREEN_SIZE);

	draw_maze(map, double_buffer, xview, yview, viewing_angle);

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
void draw_maze(map_type map, char far* screen, int xview, int yview, float viewing_angle)
{

	int sy, offset;				// pixel y position and offset
	float xd, yd;				// distance to next wall in x and y
	int grid_x, grid_y;			// coordinates of x and y grid lines
	float xcross_x, xcross_y;	// ray intersection coordinates
	float ycross_x, ycross_y;

	int xdist, ydist;
	int xmaze, ymaze;
	int distance;

	// *** The raycasting begins:

	// loop through all columns of pixels in viewport:
	for(int column = VIEWPORT_LEFT; column < VIEWPORT_RIGHT; column++)
	{
		// calculate horizontal angle of ray relative to center ray:
		float column_angle = atan((float)(column - 160) / VIEWER_DISTANCE);

		// calculate angle of ray relative to maze coordinates:
		float radians = viewing_angle + column_angle;

		// rotate endpoint of ray to viewing angle:
		int x2 = 1024 * (cos(radians));
		int y2 = 1024 * (sin(radians));

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

		int color = 15;

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

			// get x,y coordinates where ray crosses x grid line:
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

				// is there a maze cube here? If so, stop looping:
				if(map[xmaze][ymaze])
				{
					color = map[xmaze][ymaze];
					break;
				}
			}
			else	// if y grid line is closer:
			{
				// calculate maze grid coordinates of square:
				xmaze = ycross_x / 64;
				ymaze = ycross_y / 64;

				// set x and y to piont of ray intersection:
				x = ycross_x;
				y = ycross_y;

				// is there a maze cube here? If so, stop looping:
				if(map[xmaze][ymaze])
				{
					color = map[xmaze][ymaze];
					break;
				}
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
		int bot = VIEWER_DISTANCE * VIEWER_HEIGHT / distance + VIEWPORT_CENTER;

		// calculate top of wall on screen:
		int top = bot - height;

		// clip wall to viewport
		if(top < VIEWPORT_TOP)
		{
			height -= (VIEWPORT_TOP - top);
			top = VIEWPORT_TOP;
		}
		if((top + height) > VIEWPORT_BOT)
		{
			height -= (bot - VIEWPORT_BOT);
		}

		// *** Draw the wall column:

		// find video offset of top pixel in wall column:
		offset = top * 320 + column;

		// loop through all pixels in wall column:
		for(int i = 0; i < height; i++)
		{
			// set wall pixels to white:
			screen[offset] = color;

			// advance to next vertical pixel:
			offset += 320;
		}

	} // for each column in viewport

}

