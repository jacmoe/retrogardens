#include <stdio.h>
#include <dos.h>
#include <conio.h>
#include <stdlib.h>
#include <mem.h>
#include <math.h>
#include <limits.h>
#include <assert.h>
#include "screen.h"
#include "jmkbd.h"
#include "pcx.h"

#define FP16			16
#define FP20			20
#define ROUND20			((1L << 20) / 2)

#define CHECK_DIST		48L				// minimum distance from wall

enum { NO_WALL, X_WALL, Y_WALL };		// wall struck codes

#define MAX_DISTANCE	2048
#define MIN_DISTANCE	10

#define RBITMAP_WIDTH 	64
#define RBITMAP_HEIGHT 	64

#define MAP_SIZE		64
#define MAP_WIDTH		64
#define MAP_HEIGHT		64
#define MAP_MAX			MAP_WIDTH * MAP_HEIGHT
#define MAP_XMAX		MAP_WIDTH * MAP_SIZE
#define MAP_YMAX		MAP_HEIGHT * MAP_SIZE

#define MAP_XMAXLONG	(MAP_XMAX * (1L << FP16))
#define MAP_YMAXLONG	(MAP_YMAX * (1L << FP16))

enum { FALSE, TRUE };

#define CEILING_COLOR	0
#define FLOOR_COLOR		8

#define VIEW_WIDTH		240
#define VIEW_HEIGHT		120
#define VIEW_ANGLE		60
#define NUM_RBITMAPS	16
#define NUM_FACES		5
#define MAX_HEIGHT		1024
#define MIN_HEIGHT		16

#define RBITMAP_BYTES	(RBITMAP_WIDTH * RBITMAP_HEIGHT)

#define Degree_6		(int)(6L * VIEW_WIDTH / VIEW_ANGLE)
#define Degree_30		(int)(30L * VIEW_WIDTH / VIEW_ANGLE)
#define Degree_45		(int)(45L * VIEW_WIDTH / VIEW_ANGLE)
#define Degree_90		(int)(90L * VIEW_WIDTH / VIEW_ANGLE)
#define Degree_135		(int)(135L * VIEW_WIDTH / VIEW_ANGLE)
#define Degree_180		(int)(180L * VIEW_WIDTH / VIEW_ANGLE)
#define Degree_225		(int)(225L * VIEW_WIDTH / VIEW_ANGLE)
#define Degree_270		(int)(270L * VIEW_WIDTH / VIEW_ANGLE)
#define Degree_315		(int)(315L * VIEW_WIDTH / VIEW_ANGLE)
#define Degree_360		(int)(360L * VIEW_WIDTH / VIEW_ANGLE)

typedef struct
{
	unsigned char r, g, b;
} RGB_t;

typedef RGB_t DPalette[256];

typedef struct
{
	int Style,
		Column,
		Distance;
} View_t;

typedef struct
{
	int x,
		y,
		angle;
} Pov_t;

void draw_frame(void);
void get_sine_cosine(int angle);
char ray_XZ(int x, int y, int angle);
char ray_YZ(int x, int y, int angle);
int init_tables(void);
void free_mem(void);
void init_timer(void);
void restore_timer(void);
void cleanup(void);
void init_map(char* filename);
void init_bitmaps(char* filename);
void set_dpalette(DPalette d);
void read_dpalette(char* filename, DPalette d);
void init_heighttable(void);
void initialize(void);
void init_video(void);
void restore_video(void);
void blit(char* bmp, int x, int y, int w, int h);

void generate_view(void);
int hit_test(int x, int y, int angle);
double event_loop(void);

word* game_clock = (word*)0x0000046C;	// points to 18.2hz system clock
byte* double_buffer;					// Double buffer

char* screen_buffer;
char* screen;

char* face[NUM_FACES];
int mapWidth, mapHeight;
char* map;
char* xzWalls;
char* yzWalls;
unsigned videoRow[200];
unsigned char volatile scanCode;
int volatile faceIndex;
unsigned int frames = 0;
Pov_t Pov;

int heightScale = 10000;
PcxImage pcx;
char* bitmap[NUM_RBITMAPS + 1];
int heightTable[MAX_DISTANCE + 1];
long scaleTable[MAX_DISTANCE + 1];
View_t View[VIEW_WIDTH];

long cosAngle, sinAngle, rayX, rayY;

int iRayX, iRayY;

int farthestWall;

long* sine;
long* cosine;
long* tangent;
long* invTangent;
long* invCosine;
long* invSine;
long* dxTable;
long* dyTable;

void main()
{

	initialize();

	Pov.x = Pov.y = 96;	// set player location
	Pov.angle = 0;

	get_sine_cosine(Pov.angle);

	generate_view();

	double frame_rate = event_loop();

	// release memory
	cleanup();

	printf("Frame rate: %6.2f f/s\n", frame_rate);
}

void draw_frame(void)
{
	unsigned hceil;
	long scaleFactor, bmpOffset;
	char* display;
	char* p;
	char* bmp;
	int i, j, h, row;

	h = heightTable[farthestWall];	// lookup smallest height on screen
	if(h < VIEW_HEIGHT)				// is it still > viewport height?
	{
		// if not, then draw floors/ceilings
		row = (VIEW_HEIGHT - h) >> 1;
		hceil = videoRow[row + h];
		display = screen_buffer;
		for(i = 0; i < row; i++)
		{
			memset(display, CEILING_COLOR, VIEW_WIDTH);			// draw ceiling row
			memset(display + hceil, FLOOR_COLOR, VIEW_WIDTH);	// draw floor row
			display += VIEW_WIDTH;
		}
	}

	for(i = 0; i < VIEW_WIDTH; i++)
	{
		display = screen_buffer + i;		// init display buffer
		bmp = bitmap[View[i].Style];		// get ptr to bitmap
		bmp += (View[i].Column << 6);		// add in column offset
		h = heightTable[View[i].Distance];	// find wall-slice height
		row = (VIEW_HEIGHT - h) >> 1;		// calc starting row
		bmpOffset = 0;						// start with first pixel
		scaleFactor = scaleTable[View[i].Distance];	// get scaling factor
		if(row < 0)
		{
			h += row << 1;					// adjust wall-slice height
			for(j = row; j < 0; j++)		// loop until on screen
				bmpOffset += scaleFactor;	// update position in bitmap
		}
		else
			display += videoRow[row];		// point to starting row
		for(j = 0; j < h; j++)				// texture mapping loop
		{
			// copy bitmap pixel to screenbuffer
			*display =* (bmp + (unsigned)(((0xFFFF0000L & bmpOffset) >> (16))));
			display += VIEW_WIDTH;			// get next screenbuffer row
			bmpOffset += scaleFactor;		// update position in bitmap
		}
	}
	display = screen_buffer;				// get source pointer
	p = screen + 20 + SCREEN_WIDTH * 40;	// get destination pointer
	for(i = 0; i < VIEW_HEIGHT; i++)		// copy each row
	{
		memcpy(p, display, VIEW_WIDTH);		// copy row to screen
		p += SCREEN_WIDTH;
		display += VIEW_WIDTH;
	}

}

void get_sine_cosine(int angle)
{
	if(angle < Degree_270)
	{
		sinAngle = (- *(sine - angle + Degree_360));
		cosAngle = (*(sine + angle - Degree_270));
	}
	else if(angle > Degree_180)
	{
		sinAngle = (- *(sine + angle - Degree_180));
		cosAngle = (- *(sine + angle + Degree_270));
	}
	else if(angle > Degree_90)
	{
		sinAngle = (*(sine - angle + Degree_180));
		cosAngle = (- *(sine + angle - Degree_90));
	}
	else
	{
		sinAngle = (*(sine + angle));
		cosAngle = (*(sine - angle + Degree_90));
	}
}

char ray_XZ(int x, int y, int angle)
{
	char styleCode;
	int mapIndex;
	int xLocation, dx;
	long yLocation, dy;

	xLocation = x & 0xFFC0;		// get upper left corner of cell

	dy = dyTable[angle];		// pre-calc'ed value of 64*tan(angle)

	if(angle > Degree_270 || angle < Degree_90)
	{
		xLocation += MAP_SIZE;	// looking to the right
		dx = MAP_SIZE;			// positive in x direction
	}
	else
	{
		dx = - MAP_SIZE;		// negative x direction
		dy = - dy;				// opposite y direction
	}

	// calculate the y coordinate of the ray for the current square
	yLocation = (((long)xLocation - (long)x) * tangent[angle]) + ((long)y << FP16);
	while(TRUE)
	{
		if(xLocation < 0 || xLocation > MAP_XMAX ||
			yLocation < 0 || yLocation > MAP_YMAXLONG)
				break;
		mapIndex = ((yLocation >> FP16) & 0xffc0) + (xLocation >> 6);	// y / 64 * 64 + x / 64
		if((styleCode == xzWalls[mapIndex]) != 0)	// did we hit a wall?
		{
			iRayX = xLocation;		// remember ray x coordinate
			rayY = yLocation;		// remember ray y coordinate
			return styleCode;		// return wall style code
		}
		xLocation += dx;			// next x coordinate (fixed at +64 or -64)
		yLocation += dy;			// next calculated y coordinate for a delta of x
	}
	return NO_WALL;
}

char ray_YZ(int x, int y, int angle)
{
	char styleCode;
	int mapIndex;
	int yLocation, dy;
	long xLocation, dx;

	yLocation = y &0xFFC0;			// get upper left corner of cell
	dx = dxTable[angle];			// pre-calc'ed value of 64/tan(angle)

	if(angle < Degree_180)
	{
		yLocation += MAP_SIZE;		// looking down
		dy = MAP_SIZE;				// positive direction
	}
	else
	{
		dy = -MAP_SIZE;				// negative y direction
		dx = -dx;					// opposite x direction
	}

	// calculate the x coordinate for the current square
	xLocation = (((long)yLocation - (long)y) * invTangent[angle]) + ((long)x << FP16);
	while(TRUE)
	{
		if(xLocation < 0 || xLocation > MAP_XMAXLONG ||
			yLocation < 0 || yLocation > MAP_YMAX)
				break;
		mapIndex = (yLocation & 0xFFC0) + (xLocation >> (FP16 + 6));	// y / 64 * 64 + x / 64
		if((styleCode = yzWalls[mapIndex]) != 0)		// did we hit a wall?
		{
			rayX = xLocation;		// remember ray x coordinate
			iRayY = yLocation;		// remember ray y coordinate
			return styleCode;		// return wall-cube style code
		}
		xLocation += dx;			// next calculated x value for a delta of y
		yLocation += dy;			// next y coordinate (fixed at +64 or -64)
	}
	return NO_WALL;			// no y-wall was found
}

int init_tables(void)
{
	FILE* f;
	int len;

	len = sizeof(long)* Degree_360;
	sine = (long*)malloc(len / 4 + sizeof(long));
	if(sine == NULL) return FALSE;

	f = fopen("tables.dat", "rb");
	if(f == NULL) return FALSE;

	fread(sine, len / 4 + sizeof(long), 1, f);

	tangent = (long*)malloc(len);
	invTangent = (long*)malloc(len);
	invSine = (long*)malloc(len);
	invCosine = (long*)malloc(len);
	dxTable = (long*)malloc(len);
	dyTable = (long*)malloc(len);
	cosine = (long*)malloc(sizeof(long) * VIEW_WIDTH);

	if(tangent == NULL || invTangent == NULL ||
		invSine == NULL || invCosine == NULL ||
		dxTable == NULL || dyTable == NULL || cosine == NULL)
	{
		return FALSE;
	}

	fread(cosine, sizeof(long) * VIEW_WIDTH, 1, f);
	fread(tangent, len, 1, f);
	fread(invTangent, len, 1, f);
	fread(invCosine, len, 1, f);
	fread(invSine, len, 1, f);
	fclose(f);

	for(len = 0; len < Degree_360; len++)
	{
		dyTable[len] = MAP_SIZE * tangent[len];		// pre-calc 64 * tan
		dxTable[len] = MAP_SIZE * invTangent[len];	// pre-calc 64 / tan
	}

	return TRUE;	// tables were built successfully
}

void free_mem(void)
{
	int i;

	free(sine);
	free(dxTable);
	free(dyTable);
	free(tangent);
	free(invTangent);
	free(invSine);
	free(invCosine);
	free(xzWalls);
	free(yzWalls);

	// free the bitmaps
	for(i = 0; i <= NUM_RBITMAPS; i++)
	{
		free(bitmap[i]);
	}
	for(i = 0; i < NUM_FACES; i++)
	{
		free(face[i]);
	}

	// release double buffer
	free(double_buffer);
}

void init_timer(void)
{
}

void restore_timer(void)
{
}

void cleanup(void)
{
	restore_video();
	free_mem();
}

void init_map(char* filename)
{
	FILE* f;
	char mapCode;
	int i, j, pos;

	f = fopen(filename, "rt");
	assert(f != NULL);
	mapWidth = mapHeight = 64;
	map = (char*)calloc(mapWidth * mapHeight, sizeof(char));
	assert(map != NULL);
	for(i = 0; i < mapWidth * mapHeight; i++)	// read in the map file
		fscanf(f, "%x", &(map[i]));
	fclose(f);

	xzWalls = (char*)calloc((mapWidth + 1) * (mapHeight + 1), sizeof(char));
	assert(xzWalls != NULL);

	yzWalls = (char*)calloc((mapWidth + 1) * (mapHeight + 1), sizeof(char));
	assert(yzWalls != NULL);

	for(i = 0; i < mapHeight; i++)
	{										// we need to reformat the map
		for(j = 0; j < mapWidth; j++)		// so that we can look at xz and yz plane walls
		{
			pos = (i * mapWidth) * j;
			mapCode = map[pos];				// get wall-cube style code
			if(mapCode)
			{
				xzWalls[pos] = mapCode;		// store xz wall boundary
				xzWalls[pos + 1] = mapCode;	// there are 2 per wall-cube
				yzWalls[pos] = mapCode;		// store yz-wall boundary
				yzWalls[pos + mapWidth] = mapCode;	// there are 2 per wall-cube
			}
		}
	}
	free(map);	// don't need this memory anymore
}

// this routine loads the palette file, the static background bitmap,
// and the bitmap tiles listed in the definition (.def) file
void init_bitmaps(char* filename)
{
	DPalette pal;
	FILE* f;
	char str[80], fname[80];
	int i, result;

	f = fopen(filename, "rt");
	if(f == NULL)
	{
		fprintf(stderr, "\nCouldn't open %s!\n", fname);
		exit(1);
	}
	fscanf(f, "%s", str);			// get palette name from .def file
	sprintf(fname, "%s.pal", str);	// make it a filename
	read_dpalette(fname, pal);		// read in the DAC palette
	set_dpalette(pal);				// set up the VGA DAC registers

	fscanf(f, "%s", str);
	sprintf(fname, "tex\\new\\%s.pcx", str);
	result = PcxLoadImage(fname, &pcx);	// load static background
	if(result == PCX_OK)
	{
		memcpy(screen, pcx.bitmap, 64000L);
		free(pcx.bitmap);	// don't need it anymore
	}

	for(i = 0; i < NUM_FACES; i++)
	{
		sprintf(fname, "tex\\new\\face%d.pcx", i);
		result = PcxLoadImage(fname, &pcx);	// load face
		if(result == PCX_OK)
			face[i] = pcx.bitmap;
		else
			face[i] = NULL;
	}
	blit(face[faceIndex], 266, 133, 50, 45); // draw the face bitmap

	for(i = 0; i < NUM_RBITMAPS; i++)	// fill our wall bitmaps with pcx images
	{
		fscanf(f, "%s", str);	// get bitmap name from .def file
		sprintf(fname, "tex\\new\\%s.pcx", str);
		result = PcxLoadImage(fname, &pcx);	// load and decode the pcx file
		if(result != PCX_OK)
		{
			printf("\nPCX error: %d file %s\n", result, fname);
			exit(1);
		}
		if(!TransposeBitmap(pcx.bitmap, pcx.width, pcx.height))
		{
			printf("\nUnable to transpose bitmap: %s\n", fname);
			exit(1);
		}
		bitmap[i] = pcx.bitmap;		// store pointer in bitmap tile array
	}
	fclose(f);
}

void read_dpalette(char* filename, DPalette d)
{
	FILE* f;

	f = fopen(filename, "rb");
	fread(d, sizeof(DPalette), 1, f);
	fclose(f);
}

void set_dpalette(DPalette d)
{
	unsigned es;

	es = _ES;
	_ES = FP_SEG((void far*)d);
	_DX = FP_OFF((void far*)d);
	_AX = 0x1012;
	_BX = 0;
	_CX = 256;
	geninterrupt(0x10);
	_ES = es;
}

// build height and scaling tables and video offset table
void init_heighttable(void)
{
	int i;

	heightTable[0] = 0;
	scaleTable[0] = 0;

	for(i = 1; i < MAX_DISTANCE; i++)
	{
		heightTable[i] = (heightScale / i) & 0xFFFE;	// even number proportional to 1 / x
		scaleTable[i] = 64 * 65536L / heightTable[i];	// bitmap scaling values
		if(heightTable[i] > MAX_HEIGHT)					// limit max height
			heightTable[i] = MAX_HEIGHT;
	}

	videoRow[0] = 0;		// pre-calc video row memory access
	for(i = 1; i < 200; i++)
		videoRow[i] = videoRow[i - 1] + VIEW_WIDTH;
}

void initialize(void)
{
	if(!init_tables())
	{
		fprintf(stderr, "\nCould not load trig tables!\n");
		exit(1);
	}
	faceIndex = 0;
	init_heighttable();
	init_video();
	init_bitmaps("bitmaps.def");
	init_map("world.map");
	init_timer();
}

void init_video(void)
{
	screen =(char*)MK_FP(0xA000, 0);	// create pointer to VRAM
	_AX = 0x13;							// 320x200x256 mode
	geninterrupt(0x10);					// call video BIOS

	screen_buffer = (char*)malloc((long)VIEW_WIDTH * VIEW_HEIGHT);
	assert(screen_buffer != NULL);
	directvideo = 0;
}

void restore_video(void)
{
	_AX = 0x03;			// restore text mode video
	geninterrupt(0x10);
}

void blit(char* bmp, int x, int y, int w, int h)
{
	char* src;
	char* dest;
	int i;

	src = bmp;
	dest = screen + SCREEN_WIDTH * y + x;
	for(i = 0; i < h; i++)
	{
		memcpy(dest, src, w);
		src += w;
		dest += SCREEN_WIDTH;
	}
}

void generate_view(void)
{
	int i, distance, rayAngle;
	unsigned char yStyleCode, styleCode;
	unsigned long rayLength, yRayLength;
	unsigned int wallColumn, yWallColumn;
	long dx, dy;

	rayAngle = Pov.angle - Degree_30;	// start by looking 30 to our left

	if(rayAngle < 0)					// keep angle within table bounds
		rayAngle += Degree_360;
	farthestWall = 0;					// no farthest wall yet

	for(i = 0; i < VIEW_WIDTH; i++)		// cast two rays for each video column
	{
		rayLength = LONG_MAX;			// start with an impossible distance

		// don't cast an xz ray if it is not possible to hit an xz wall
		if(rayAngle != 0 && rayAngle != Degree_180)
		{
			yStyleCode = ray_YZ(Pov.x, Pov.y, rayAngle);		// cast xz ray
			if(yStyleCode)										// did we hit a wall?
			{
				// use the x-intercept to find the wall-slice column
				yWallColumn = (unsigned)(rayX >> FP16) & 0x3F;
				if(iRayY > Pov.y)								// looking south
					yWallColumn = 63 - yWallColumn;				// reverse column
				dy = iRayY - Pov.y;
				yRayLength = (dy * invSine[rayAngle]) >> 14;

				if(yRayLength < rayLength)			// use the shorter ray
				{
					rayLength = yRayLength;
					styleCode = yStyleCode;
					wallColumn = yWallColumn;
				}
			}
		}

		if(wallColumn < 64)							// a wall was found (either x or y)
		{
			rayLength *= cosine[i];					// cosine correct distance
			rayLength += ROUND20;					// round up to fixed point
			distance = (int)(rayLength >> FP20);	// convert to an int
			if(distance < MIN_DISTANCE)				// check for min distance
				distance = MIN_DISTANCE;
			if(distance >= MAX_DISTANCE)			// check for max distance
				distance = MAX_DISTANCE - 1;

			// save the wall-slice data
			View[i].Distance = distance;
			View[i].Style = (styleCode & 0x3F);
			View[i].Column = wallColumn;
			if(distance > farthestWall)				// is new distance the farthest?
				farthestWall = distance;			// update farthest distance
		}
		rayAngle++;									// get next ray angle
		if(rayAngle >= Degree_360)					// keep angle within tables
			rayAngle -= Degree_360;
	}
	draw_frame();									// use View[] to draw the entire screen
	blit(face[faceIndex], 266, 133, 50, 45);		// draw updated face
}

// validates player moves
int hit_test(int x, int y, int angle)
{
	int wall;
	unsigned long distance, ydistance;
	int dx, dy;

	distance = LONG_MAX;		// set to a very long distance

	// correct for angles where sin = cos, push the angle to one side
	// otherwise we can get false readings
	if(angle == Degree_45 || angle == Degree_135 ||
		angle == Degree_225 || angle == Degree_315)
			angle++;

	// don't cast a xz ray if it can't possibly hit an xz wall
	if(angle != Degree_90 && angle != Degree_270)
	{
		if(ray_XZ(x, y, angle))		// cast a xz ray
		{
			// we hit something
			dx = iRayX - x;			// get ray x component
			distance = (dx * invCosine[angle]) >> 14;	// calc distance
			wall = X_WALL;			// set wall struck code
		}
	}

	// don't cast a yz ray if it can't possibly hit a yz wall
	if(angle != 0 && angle != Degree_180)
	{
		if(ray_YZ(x, y, angle))		// cast a yz ray
		{
			// we hit something
			dy = iRayY - y;			// get ray y component
			ydistance = (dy * invSine[angle]) >> 14;	// calc distance
			if(ydistance < distance)	// use the shorter ray
			{
				distance = ydistance;	// use y ray distance
				wall = Y_WALL;			// set wall struck code
			}
		}
	}

	if(wall)				// a wall was hit
	{
		distance *= cosine[VIEW_WIDTH / 2];	// cosine correct distance
		distance += ROUND20;				// round-up fixed point
		distance >>= FP20;					// convert back to integer
		if(distance > CHECK_DIST)			// check distance
			wall = NO_WALL;					// it's safe to move
	}
	return wall;
}

double event_loop(void)
{
	int dx, dy, revAngle, result;

	Keyboard kb;

	double start_time = *game_clock;
	double frames = 0;

	// Wait for the user to hit escape key
	while(!kb.wasPressed(esckey))
	{
		//double time = 0; // time of current frame
		//double oldTime = 0; // time of previous frame

		//double moveFactor = 10.0;
		//double rotateFactor = 3.0;

		//oldTime = time;
		//time = *game_clock;
		//double frameTime = (time - oldTime) / 18.2;

		//double moveSpeed = frameTime * moveFactor;
		//double rotSpeed = frameTime * rotateFactor;

		// move forwards if no wall in front of you
		if(kb.isKeyDown(uparrow))
		{
			dx = (int)(cosAngle >> 12); // get x-component
			dy = (int)(sinAngle >> 12);	// get y-component
			result = hit_test(Pov.x, Pov.y, Pov.angle);	// check for all walls
			if(result == X_WALL)
			{
				// we hit an x-wall
				dx = 0;
				if(Pov.angle < Degree_180)
					revAngle = Degree_90;
				else
					revAngle = Degree_270;
				result = hit_test(Pov.x, Pov.y, revAngle);
			}
			if(result == Y_WALL)
			{
				// we hit an y-wall
				dx = 0;
				if(Pov.angle > Degree_270 || Pov.angle < Degree_90)
					revAngle = 0;
				else
					revAngle = Degree_180;
				result = hit_test(Pov.x, Pov.y, revAngle);
			}
			if(result == NO_WALL)
			{
				// no walls were hit, so we can move
				Pov.x += dx;
				Pov.y += dy;
			}
		}
		// move backwards if no wall in front of you
		if(kb.isKeyDown(dnarrow))
		{
			dx = (int)(cosAngle >> 12);
			dy = (int)(sinAngle >> 12);
			revAngle = Pov.angle + Degree_180;
			if(revAngle >= Degree_360)
				revAngle -= Degree_360;
			result = hit_test(Pov.x, Pov.y, revAngle);
			if(result == X_WALL)
			{
				dx = 0;
				if(revAngle < Degree_180)
					revAngle = Degree_90;
				else
					revAngle = Degree_270;
				result = hit_test(Pov.x, Pov.y, revAngle);
			}
			if(result == Y_WALL)
			{
				dy = 0;
				if(revAngle > Degree_270 || revAngle < Degree_90)
					revAngle = 0;
				else
					revAngle = Degree_180;
				result = hit_test(Pov.x, Pov.y, revAngle);
			}
			if(result == NO_WALL)
			{
				Pov.x -= dx;
				Pov.y -= dy;
			}
		}
		// rotate to the right
		if(kb.isKeyDown(rtarrow))
		{
			Pov.angle += Degree_6;
			if(Pov.angle >= Degree_360)
				Pov.angle -= Degree_360;
			get_sine_cosine(Pov.angle);
		}
		// rotate to the left
		if(kb.isKeyDown(lfarrow))
		{
			Pov.angle -= Degree_6;
			if(Pov.angle < 0)
				Pov.angle += Degree_360;
			get_sine_cosine(Pov.angle);
		}

		generate_view();
		frames++;

	} // while not escape key pressed
	double end_time = *game_clock;

	return frames * 18.2 / (end_time - start_time);
}
