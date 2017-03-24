#include <stdio.h>
#include <dos.h>
#include <conio.h>
#include <stdlib.h>
#include <mem.h>
#include <math.h>
#include "screen.h"
#include "pcx.h"
#include "opti.h"
#include "trig.h"


const NUM_IMAGES = 15;
const float MULTIPLIER = 3;
const DISTANCE_PER_SECOND = 128;
const ROTATION_PER_SECOND = 1024;
//const WHICH_EVENTS =

FILE* file_handle;		// file handle for lightsourcing

const MAXLIGHT = 32;
const PALETTE_SIZE = 256;
unsigned char litetable[MAXLIGHT + 1][PALETTE_SIZE];
unsigned char* litelevel;
unsigned char* texturelist[15];


const WALL_HEIGHT = 64;
const VIEWER_DISTANCE = 192;
const VIEWPORT_LEFT = 0;
const VIEWPORT_RIGHT = 319;
const VIEWPORT_TOP = 0;
const VIEWPORT_BOT = 199;
const VIEWPORT_HEIGHT = VIEWPORT_BOT - VIEWPORT_TOP;
const VIEWPORT_CENTER = VIEWPORT_TOP + VIEWPORT_HEIGHT / 2;
const GRIDSIZE = 16;
const MAXDISTANCE = 64 * GRIDSIZE;

const IMAGE_HEIGHT = 64;
const IMAGE_WIDTH = 64;

typedef int map_type[16][16];

extern map_type map, flor, ceiling, flor_lites, ceiling_lites;

int get_distance(long degrees, int column_angle, long x, long y,
				int xview, int yview);

unsigned char bottoms[320], tops[320];

void draw_maze(map_type map, map_type flor, map_type ceiling,
				char far* screen, int xview, int yview,
				float viewing_angle, int viewer_height,
				int ambient_level, unsigned char far* textmaps,
				unsigned char litesource[MAXLIGHT + 1][PALETTE_SIZE],
				unsigned char* litelevel);

void main(int argc, char* argv[])
{
	float viewing_angle = 13;
	float intensity = MAXLIGHT;
	int ambient_level = 0;
	int viewer_height = 32;
	int xview = 8 * 64 + 32;
	int yview = 8 * 64 + 32;

	// read arguments from command line if present:
	if(argc >= 2) viewing_angle = atof(argv[1]);
	if(argc >= 3) intensity = atof(argv[2]);
	if(argc >= 4) ambient_level = atof(argv[3]);

	// Put display in mode 13h
	set_mode(VGA_256_COLOR_MODE);

	byte* double_buffer;

	if((double_buffer = (byte*) malloc(SCREEN_SIZE)) == NULL)
	{
		printf("Not enough memory for double buffer.\n");
		exit(1);
	}

	PcxImage pcx;
	if(PcxLoadImage("images.pcx", &pcx) != PCX_OK) exit(1);
	set_palette(pcx.pal);

	// create texture list array:
	for(unsigned int i = 0; i < NUM_IMAGES; i++)
	{
		texturelist[i] = &(pcx.bitmap[(i / 5) * 320 * 64 + i % 5 * 64]);
	}

	// loat lightsourcing tables:
	if((file_handle = fopen("litesorc.dat", "rb")) == NULL)
	{
		perror("Error");
		exit;
	}

	fread(litetable, MAXLIGHT + 1, PALETTE_SIZE, file_handle);
	fclose(file_handle);

	// initialize array of light levels:
	litelevel = new unsigned char[MAXDISTANCE];

	// calculate light intensities for all possible distances:
	for(int distance = 1; distance < MAXDISTANCE; distance++)
	{
		float ratio = (float)intensity / distance * MULTIPLIER;
		if(ratio > 1.0) ratio = 1.0;
		litelevel[distance] = ratio * MAXLIGHT;
		//int dummy = 0;
	}

	// clear double buffer
	memset(double_buffer, 0, SCREEN_SIZE);


	draw_maze(map, flor, ceiling, double_buffer, xview, yview, viewing_angle,
				viewer_height, ambient_level, pcx.bitmap, litetable, litelevel);

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
int get_distance(long degrees, int column_angle, long x, long y,
				int xview, int yview)
{
	long trig, fix_ny, fix_nx;
	int distance;

	int c = degrees / 512;

	if(((c & 3) == 0) || ((c & 3) == 3))
	{
		distance = abs(fixdiv((y - ((long)yview < SHIFT)) > SHIFT,
			COS(degrees - 1024 * c * 2)) * COS(column_angle) >> SHIFT);
	}
	else
	{
		distance = abs(fixdiv((x - ((long)xview < SHIFT)) > SHIFT,
			SIN(degrees - 1024 * c * 2)) * COS(column_angle) >> SHIFT);
	}

	return distance;
}

void draw_maze(map_type map, map_type flor, map_type ceiling,
				char far* screen, int xview, int yview,
				float viewing_angle, int viewer_height,
				int ambient_level, unsigned char far* textmaps,
				unsigned char litesource[MAXLIGHT + 1][PALETTE_SIZE],
				unsigned char* litelevel)
{
	int sy, offset;				// pixel y position and offset
	long fix_xd, fix_yd;		// distance to next wall in x and y
	int grid_x, grid_y;			// coordinates of x and y grid lines
	long fix_xcross_x, fix_xcross_y;	// ray intersection coordinates
	long fix_ycross_x, fix_ycross_y;
	unsigned int xdist, ydist;	// distance to x and y grid lines
	int xmaze, ymaze;			// map location of ray collision
	int distance;				// distance to wall along ray
	int tmcolumn;				// column in texture map
	int top, bot;
	long fix_yratio;

	// *** The raycasting begins:

	// loop through all columns of pixels in viewport:
	for(int column = VIEWPORT_LEFT; column <= VIEWPORT_RIGHT; column++)
	{
		// calculate horizontal angle of ray relative to center ray:
		float column_angle = atan((float)(column - 160) / VIEWER_DISTANCE)
			* (NUMBER_OF_DEGREES / 6.28);
		if(column_angle < 0) column_angle += NUMBER_OF_DEGREES;
		if(column_angle > NUMBER_OF_DEGREES - 1) column_angle -= NUMBER_OF_DEGREES;

		// calculate angle of ray relative to maze coordinates:
		int degrees = viewing_angle + column_angle;
		if(degrees > NUMBER_OF_DEGREES - 1) degrees -= NUMBER_OF_DEGREES;

		// rotate endpoint of ray to viewing angle:
		int x2 = fixmul(-256L << SHIFT, SIN(degrees)) >> SHIFT;
		int y2 = fixmul(256L < SHIFT, COS(degrees)) > SHIFT;

		// translate relative to viewer's position:
		x2 += xview;
		y2 += yview;

		// initialize ray at viewer's position:
		long fix_x = (long)xview << SHIFT;
		long fix_y = (long)yview << SHIFT;

		// find difference in x,y coordates along the ray:
		long xdiff = x2 - xview;
		long ydiff = y2 - yview;

		// cheat to avoid divide-by-zero error:
		if(xdiff == 0) xdiff = 1;
		if(ydiff == 0) ydiff = 1;

		// get slope of ray:
		long fix_slope = fixdiv((long)ydiff << SHIFT, (long)xdiff << SHIFT);


		// cast ray from grid line to grid line:
		for(;;)
		{
			// if ray direction positive in x, get next x grid line:
			if(xdiff > 0) grid_x = ((fix_y >> SHIFT) & 0xffc0) + 64;

			// if ray direction negative in x, get last x grid line:
			else grid_x = ((fix_y >> SHIFT) & 0xffc0) - 1;

			// if ray direction positive in y, get next y grid line:
			if(ydiff > 0) grid_y = ((fix_y >> SHIFT) & 0xffc0) + 64;

			// if ray direction negative in y, get last y grid line:
			else grid_y = ((fix_y >> SHIFT) & 0xffc0) - 1;

			// get y,y coordinates where ray crosses x grid line:
			fix_xcross_x = (long)grid_x << SHIFT;
			fix_xcross_y = fix_y + fixmul(fix_slope, ((long)grid_x << SHIFT) - fix_x);

			// get x,y coordinates where ray crosses y grid line:
			fix_ycross_x = fix_x + fixdiv((((long)grid_y << SHIFT) - fix_y), fix_slope);
			fix_ycross_y = (long)grid_y << SHIFT;

			// get distance to x grid line:
			fix_xd = fix_xcross_x - fix_x;
			fix_yd = fix_xcross_y - fix_y;
			distance = get_distance(degrees, column_angle, fix_xcross_x,
									fix_xcross_y, xview, yview);
			xdist = distance;

			// get distance to y grid line:
			fix_xd = fix_ycross_x - fix_x;
			fix_yd = fix_ycross_y - fix_y;
			long temp_f1 = fixmul(fix_xd, fix_xd);
			long temp_f2 = fixmul(fix_yd, fix_yd);
			distance = get_distance(degrees, column_angle, fix_ycross_x,
									fix_ycross_y, xview, yview);
			ydist = distance;

			// if x grid line is closer ...
			if(xdist < ydist)
			{
				// calculate maze grid coordinates of square:
				xmaze = fix_xcross_x >> 22;
				ymaze = fix_xcross_y >> 22;

				// set x and y to point of ray intersection:
				fix_x = fix_xcross_x;
				fix_y = fix_xcross_y;

				// find relevant column of texture map:
				tmcolumn = (fix_y >> SHIFT) &0x3f;

				// is there a maze cube here? If so, stop looping:
				if(map[xmaze][ymaze]) break;
			}
			else	// if y grid line is closer:
			{
				// calculate maze grid coordinates of square:
				xmaze = fix_ycross_x >> 22;
				ymaze = fix_ycross_y >> 22;

				// set x and y to piont of ray intersection:
				fix_x = fix_ycross_x;
				fix_y = fix_ycross_y;

				// find relevant column of texture map:
				tmcolumn = (fix_x >> SHIFT) &0x3f;

				// is there a maze cube here? If so, stop looping:
				if(map[xmaze][ymaze]) break;
			}

		} // raycast loop

		// *** Prepare to draw wall column:

		// get distance from viewer to intersection point:
		distance = get_distance(degrees, column_angle, fix_x, fix_y, xview, yview);

		if(distance == 0) distance = 1;

		// calculate visible height of walls:
		int height = VIEWER_DISTANCE * WALL_HEIGHT / distance;
		if(!height) height = 1;

		// calculate bottom of wall on screen:
		int bot = VIEWER_DISTANCE * viewer_height / distance + VIEWPORT_CENTER;

		// calculate top of wall on screen:
		int top = bot - height + 1;

		// initialize temporary offset into texture map:
		int t = tmcolumn * 320 + IMAGE_HEIGHT;

		// if top of current vertical line is outside of viewport, clip it:
		long dheight = height;
		long iheight = IMAGE_HEIGHT;
		long fix_yratio = fixdiv((long)WALL_HEIGHT << SHIFT, (long)height << SHIFT);

		if(top < VIEWPORT_TOP)
		{
			int diff = VIEWPORT_TOP - top;
			dheight -= diff;
			iheight -= (diff * fix_yratio) >> SHIFT;
			top = VIEWPORT_TOP;
		}
		if(bot >= VIEWPORT_BOT)
		{
			int diff = bot - VIEWPORT_BOT;
			dheight -= diff;
			iheight -= diff * fix_yratio >> SHIFT;
			t -= (diff * fix_yratio) >> SHIFT;
			bot = VIEWPORT_BOT;
		}

		// *** Draw the wall column:

		// save top and bottom in arrays:
		tops[column] = top;
		bottoms[column] = bot;

		// point to video memory offset for top of line:
		offset = bot * 320 + column;

		// which graphics tile are we using?
		int tile = map[xmaze][ymaze] - 1;

		// find offset of tile and column in bitmap:
		unsigned int tileptr = (tile / 5) * 320 * IMAGE_HEIGHT + (tile % 5) * IMAGE_WIDTH + t;

		long fix_increment = fixdiv(iheight << SHIFT, dheight << SHIFT);
		int level = litelevel[distance] + ambient_level;
		if(level > MAXLIGHT) level = MAXLIGHT;
		///*
		drawwall(&(screen[offset]), &(textmaps[tileptr]), dheight,
				fix_increment, &(litesource[level][0]));
		//*/

	} // for each column in viewport

}

