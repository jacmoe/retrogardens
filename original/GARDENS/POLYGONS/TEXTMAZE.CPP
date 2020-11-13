
// TEXTMAZE.CPP Version 1.0
// An animated tour of a 3D texture-mapped maze
//
// Written by Christopher Lampton October 19, 1993
//   for GARDENS OF IMAGINATION (Waite Group Press)

#include  <stdio.h>
#include  <dos.h>
#include  <conio.h>
#include  <stdlib.h>
#include  <time.h>
#include  "screen.h"
#include  "io.h"
#include  "evntmgr1.h"
#include  "bitmap.h"
#include  "pcx.h"
#include  "targa.h"
#include  "polytext.h"

#define   FRAMES_PER_SECOND 5
#define   WHICH_EVENTS KEYBOARD_EVENTS+MOUSE_EVENTS+JOYSTICK_EVENTS
#define   POLYWIDTH 96
#define   POLYHEIGHT 79

// Function prototypes:

void display_slice(int imagenum,int leftx,int rightx,
										char far *screen);
void drawbg(int num,char far *screen);
void drawmaze(int xshift,int yshift,int lview,int lrview,
										char *screen);

const VIEW_LIMIT = 4; // Number of squares visible both
											// forward and to the side.
											// (Should not be changed without
											// adding additional bitmaps.)

const XWINDOW = 110;  // Upper left corner of view window
const YWINDOW = 0;
const WWIDTH = 188;
const WHEIGHT = 120;

// Buffers for compass picture and background:

pcx_struct compasses,pcx;
tga_struct bg[2],wall;
char far bitmap[95*79];

// Array of compass faces:

unsigned char far *compass_face[4];

// MAP OF THE MAZE
//
// Each element represents a physical square within the maze.
// A value of 0 means the square is empty, a nonzero value
// means that the square is filled with a solid cube. Map
// must be designed so that no square can be viewed over a
// greater distance than that defined by VIEW_LIMIT.

unsigned char maze[16][16]={
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	{1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,1},
	{1,0,1,0,0,0,1,0,0,0,1,0,1,1,0,1},
	{1,0,1,1,1,1,1,0,0,0,1,0,0,0,1,1},
	{1,0,0,0,0,1,0,0,0,0,1,0,1,0,1,1},
	{1,0,0,0,0,1,0,1,1,1,1,0,0,0,1,1},
	{1,1,1,0,0,1,1,0,1,0,0,1,1,0,1,1},
	{1,0,1,0,1,1,1,0,0,0,1,0,0,0,1,1},
	{1,0,1,0,0,0,1,1,1,0,0,0,1,1,1,1},
	{1,0,0,1,1,0,1,0,1,1,1,0,1,1,1,1},
	{1,1,0,0,0,0,1,0,1,0,0,0,0,0,1,1},
	{1,0,0,0,1,1,1,0,1,0,1,1,1,0,1,1},
	{1,0,1,1,1,0,1,0,0,0,0,1,0,0,0,1},
	{1,0,0,0,1,0,1,1,1,1,0,1,0,0,0,1},
	{1,1,0,0,0,1,1,0,0,0,0,1,0,0,0,1},
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};

typedef struct xy {
	int x,y;
};

// Directional increments for north,east,south,west
// respectively:

struct xy forward[4]={{-1,0},{0,1},{1,0},{0,-1}};
struct xy left[4]={{0,-1},{-1,0},{0,1},{1,0}};
struct xy right[4]={{0,1},{1,0},{0,-1},{-1,0}};

// Viewer's starting position:

struct xy pos={3,9};

// Structure to hold polygon data:

tpolytype tpoly;

// Horizontal screen positions corresponding to
// visible intersections on the maze grid:

int matrix[VIEW_LIMIT+2][VIEW_LIMIT*2+2]={
	{187,187,187,187,187,  0,  0,  0,  0,  0},
	{187,187,187,238,143, 47,-49,  0,  0,  0},
	{187,187,187,179,123, 66, 10,  0,  0,  0},
	{187,187,187,158,117, 73, 31,  0,  0,  0},
	{187,187,177,144,111, 78, 45, 12,  0,  0},
	{187,185,159,133,107, 82, 56, 30,  4,  0}
};
int top[VIEW_LIMIT+2]={-19,20,36,42,46,49};
int bottom[VIEW_LIMIT+2]={138,99,83,77,73,70};

int dir=1;  // Viewer's current heading in maze,
						// where 0=north,1=east,2=south,3=west

long ticks_per_frame; // Number of clock ticks per
											// frame of animation

void main(void)
{
	event_struct events;
	struct xy newpos;

  // Initialize event selector:

  int which_events = WHICH_EVENTS;

	// Create pointer to video memory:

	char far *screen=(char far *)MK_FP(0xa000,0);

	// Create offscreen video buffer:

	char far *screen_buffer=new unsigned char [64000];
//	char far *screen_buffer=(char far *)MK_FP(0xa000,0);

  // Load compass picture:

	if (loadPCX("compass.pcx",&compasses)) exit(1);

	// Load compass faces into array:

	for (int num=0; num<4; num++) {
		compass_face[num]=new unsigned char[42*41];
		grab(17+num*50,18,42,41,320,compasses.image,compass_face[num]);
	}

	// Dispose of compass picture:

	delete compasses.image;

	// Load background image:

	if (loadTGA("empty.tga",&bg[0])) exit(1);
	if (loadTGA("empty.tga",&bg[1])) exit(1);

	// Load wall bitmap:

	if (loadTGA("front1.tga",&wall)) exit(1);

	// Allot memory for wall bitmap:

	tpoly.textmap=new char[POLYWIDTH*POLYHEIGHT];

	// Grab wall image in array:

	grab(47,20,POLYWIDTH,POLYHEIGHT,188,wall.image,tpoly.textmap);

	// Initialize event manager:

	init_events();

	// Calibrate the user's joystick:

  if (which_events & JOYSTICK_EVENTS) {

    // If joystick present, perform calibration:

    if (joystick_present()) {
      clrscr();
      printf("\nCenter your joystick and press button ");
      printf("one.\n");
      setcenter(); // Calibrate the center position
      printf("Move your joystick to the upper lefthand ");
      printf("corner and press button one.\n");
      setmin();    // Calibrate the minimum position
      printf("Move your joystick to the lower righthand ");
      printf("corner and press button one.\n");
      setmax();    // Calibrate the maximum position
    }

    // If joystick not present, toggle JOYSTICK_EVENTS off:

    else which_events ^= JOYSTICK_EVENTS;
  }

	// Save previous video mode:

	int oldmode=*(int *)MK_FP(0x40,0x49);

	// Put display in mode 13h:

	setmode(0x13);

	// Set palette to PCX palette:

	setpalette(bg[0].color_map);

	// Clear display:

	cls(screen_buffer);

	// Put background image into buffer:

//  for (i=0; i<64000; i++)
//    screen_buffer[i]=bg.image[i];

	// Set number of ticks per frame:

	ticks_per_frame = CLK_TCK / FRAMES_PER_SECOND;

	// Initialize the frame timer:

	clock_t lastframe=clock();

	// Make sure we get at least one frame
	// into the maze:

	events.quit_game=0;

	// Put initial image of maze in screen buffer:

  drawbg(0,screen_buffer);
	drawmaze(0,0,0,WWIDTH-1,screen_buffer);

	// Put screen buffer on screen:

//  blitscreen(screen_buffer);

  // Point to first bg buffer:

  int bgnum=0;

	// Let's go for a walk in the maze:

	while(!events.quit_game) {

		// Draw the maze in screen buffer:

    clrwin(XWINDOW,YWINDOW,WWIDTH,WHEIGHT,screen_buffer);
    drawbg(bgnum,screen_buffer);
		drawmaze(0,0,0,WWIDTH-1,screen_buffer);

		// Move screen buffer to screen:

		putwindow(XWINDOW,YWINDOW,WWIDTH,WHEIGHT,screen_buffer);

		// Put compass next to maze window:

    blit(20,35,42,41,screen,compass_face[dir]);

		// Pause until time for next frame:

		while ((clock()-lastframe)<ticks_per_frame) ;

		// Start timing another frame:

		lastframe=clock();

		// Move viewer according to input events:

		getevent(which_events,&events);

		// Do we want to move forward?

		if (events.go_forward) {
			newpos.x=pos.x+forward[dir].x;
			newpos.y=pos.y+forward[dir].y;
			if (!maze[newpos.x][newpos.y]) {
				 pos.x=newpos.x;
				 pos.y=newpos.y;
			}
		}

		// ...or do we want to move backward?

		else if (events.go_back) {
			newpos.x=pos.x-forward[dir].x;
			newpos.y=pos.y-forward[dir].y;
			if (!maze[newpos.x][newpos.y]) {
				 pos.x=newpos.x;
				 pos.y=newpos.y;
			}
		}

		// Do we want to turn left?

		if (events.go_left) {
			--dir;
			if (dir<0) dir=3;
      bgnum++;
      if (bgnum>1) bgnum=0;
		}

		// ...or do we want to turn right?

		else if (events.go_right) {
			dir++;
			if (dir>3) dir=0;
      bgnum--;
      if (bgnum<0) bgnum=1;
		}
	}

	// Terminate event manager:

	end_events();

	// Release memory:

	delete tpoly.textmap;
	delete wall.image;
	delete wall.color_map;
	if (wall.ID) delete wall.ID:
	for (int ii=0;ii<2;ii++) {
	   delete bg[ii].image;
	   delete bg[ii].color_mode;
	   if (bg[ii].ID) delete bg[ii].ID;
	}
  	for (int nm=0; nm<4; delete compass_face[nm++])  ;
	delete screen_buffer;

	// Restore old video mode:

	setmode(oldmode);
}

void drawbg(int num,char far *screen)
{

  unsigned int offset1=YWINDOW*320+XWINDOW;
  unsigned int offset2=0;
  for (int i=0; i<WHEIGHT; i++) {
    for (int j=0; j<WWIDTH; j++)
      screen[offset1++]=bg[num].image[offset2++];
    offset1+=(320-WWIDTH);
  }
}

void drawmaze(int xshift,int yshift,int lview,int rview,char *screen)

// Recursive function to draw left, right and center walls for the
// maze square at POS.X+XSHIFT, POS.Y+YSHIFT, as viewed from POS.X,
// POS.Y, within the screen window bordered by horizontal coordinates
// LVIEW and RVIEW on the left and right respectively. When called
// with XSHIFT and YSHIFT set to 0, draws entire maze as viewed
// from POS.X,POS.Y out to VIEW_LIMIT squares.

{
	int vleft,vright,vwidth;

	// Return if we've reached the recursive limit:

	if ((abs(xshift) > VIEW_LIMIT) || (abs(yshift) > VIEW_LIMIT))
		 return;

	// Calculate coordinates of square to left:

	int sx = pos.x + (xshift+1) * left[dir].x + yshift * forward[dir].x;
	int sy = pos.y + (xshift+1) * left[dir].y + yshift * forward[dir].y;

	// Get left-right extent of left part of view:

	vleft=lview;
	vright=matrix[yshift+1][xshift+VIEW_LIMIT+1];
	if (vright>rview) vright=rview;
	vwidth=vright-vleft;

	if (vwidth>0) {

		// If the square to the left is occupied,
		// draw right side of cube:

		if (maze[sx][sy]) {

			tpoly.x[0]=matrix[yshift][xshift+VIEW_LIMIT+1]+XWINDOW;
			tpoly.y[0]=top[yshift]+YWINDOW;
			tpoly.x[1]=matrix[yshift+1][xshift+VIEW_LIMIT+1]+XWINDOW;
			tpoly.y[1]=top[yshift+1]+YWINDOW;
			tpoly.y[2]=bottom[yshift+1]+YWINDOW;
			tpoly.y[3]=bottom[yshift]+YWINDOW;
			tpoly.wtext=POLYWIDTH;
			tpoly.htext=POLYHEIGHT;
			polytext(&tpoly,vleft+XWINDOW,YWINDOW,vright+XWINDOW,
							YWINDOW+WHEIGHT,screen);
		}

		// Else call function recursively to draw the view from
		// next square to the left:

		else drawmaze(xshift+1,yshift,vleft,vright,screen);

	}

	// Calculate coordinates of square directly ahead:

	sx = pos.x + xshift * left[dir].x + (yshift+1) * forward[dir].x;
	sy = pos.y + xshift * left[dir].y + (yshift+1) * forward[dir].y;


	// Get left-right extent of center part of view:

	vleft=matrix[yshift+1][xshift+VIEW_LIMIT+1];
	vright=matrix[yshift+1][xshift+VIEW_LIMIT];
	if (vleft<lview) vleft=lview;
	if (vright>rview) vright=rview;
	vwidth=vright-vleft;

	if (vwidth>0) {

		// If the square directly ahead is occupied,
		// draw front of cube:

		if (maze[sx][sy]) {
			tpoly.x[0]=matrix[yshift+1][xshift+VIEW_LIMIT+1]+XWINDOW;
			tpoly.y[0]=top[yshift+1]+YWINDOW;
			tpoly.x[1]=matrix[yshift+1][xshift+VIEW_LIMIT]+XWINDOW;
			tpoly.y[1]=top[yshift+1]+YWINDOW;
			tpoly.y[2]=bottom[yshift+1]+YWINDOW;
			tpoly.y[3]=bottom[yshift+1]+YWINDOW;
			tpoly.wtext=POLYWIDTH;
			tpoly.htext=POLYHEIGHT;
			polytext(&tpoly,vleft+XWINDOW,YWINDOW,vright+XWINDOW,
							YWINDOW+WHEIGHT,screen);
		}

		// Else call function recursively to draw the view from
		// next square forward:

		else drawmaze(xshift,yshift+1,vleft,vright,screen);
	}

	// Calculate coordinates of square to right:

	sx = pos.x + (xshift-1) * left[dir].x + yshift * forward[dir].x;
	sy = pos.y + (xshift-1) * left[dir].y + yshift * forward[dir].y;

	// Get left-right extent of right part of view:

	vleft=matrix[yshift+1][xshift+VIEW_LIMIT];
	if (vleft<lview) vleft=lview;
	vright=rview;
	vwidth=vright-vleft;

	if (vwidth>0) {

		// If the square to the right is occupied,
		// draw left side of cube:

		if (maze[sx][sy]) {
			tpoly.x[0]=matrix[yshift+1][xshift+VIEW_LIMIT]+XWINDOW;
			tpoly.y[0]=top[yshift+1]+YWINDOW;
			tpoly.x[1]=matrix[yshift][xshift+VIEW_LIMIT]+XWINDOW;
			tpoly.y[1]=top[yshift]+YWINDOW;
			tpoly.y[2]=bottom[yshift]+YWINDOW;
			tpoly.y[3]=bottom[yshift+1]+YWINDOW;
			tpoly.wtext=POLYWIDTH;
			tpoly.htext=POLYHEIGHT;
			polytext(&tpoly,vleft+XWINDOW,YWINDOW,vright+XWINDOW,
							YWINDOW+WHEIGHT,screen);
		}

		// Else call function recursively to draw the view from
		// next square to the right:

		else drawmaze(xshift-1,yshift,vleft,vright,screen);
	}
}

