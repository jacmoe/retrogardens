
// OPTDEMO.CPP
//
// Calls optimized raycasting function to draw lightsourced
// view of maze.
//
// Written by Christopher Lampton for
// Gardens of Imagination (Waite Group Press,1994)

#include  <stdio.h>
#include	<dos.h>
#include	<conio.h>
#include  <stdlib.h>
#include  <math.h>
#include  "screen.h"
#include  "optdemo.h"
#include	"pcx.h"
#include  "raycast.h"
#include  "evntmgr1.h"
#include  "bitmap.h"
#include  "io.h"
#include  "trig.h"
#include  "types.h"
#include  "htimer.h"
#include  "fixpoint.h"

// Function prototypes:

void PrintFrameRate();
void near AddFrameTime(long elapsedtime);

// Constant definitions:

const MAXFRAMES=500;
const NUM_IMAGES=15;
const float MULTIPLIER=3;
const DISTANCE_PER_SECOND=128;
const ROTATION_PER_SECOND=1024;
const WHICH_EVENTS=KEYBOARD_EVENTS+MOUSE_EVENTS+JOYSTICK_EVENTS;

map_type walls={
 { 2, 2, 2, 2, 7, 4, 7, 2, 2, 2, 2, 2, 2, 2, 2, 2},
 { 9, 2, 0, 2, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 2},
 { 9, 0, 0, 0, 0, 0, 0, 2, 8, 0, 0, 0, 0, 0, 0, 2},
 { 9, 0, 0, 0, 0, 0, 0, 0, 8, 6, 0, 0, 0, 0, 0, 2},
 { 9, 0, 0, 0, 0, 0, 0, 0, 0, 6, 2, 2, 0, 0, 0, 2},
 { 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 0, 0, 0, 2},
 { 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 9, 9, 0, 0, 2},
 { 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 2},
 { 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 9, 9, 0, 0, 2},
 { 5, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 0, 0, 0, 2},
 { 2, 2, 2, 0, 0, 0, 2, 2, 0, 2, 0, 0, 0, 0, 0, 2},
 { 7, 0, 0, 0, 0, 0, 2, 2, 0, 2, 0, 0, 0, 0, 0, 2},
 { 7, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 2},
 { 7, 0, 0, 0, 0, 0, 0, 2, 0, 2, 0, 0, 0, 0, 0, 2},
 { 7, 7, 7, 7, 7, 0, 0, 2, 0, 2, 0, 0, 0, 0, 0, 2},
 { 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2}
};

map_type flor={
 { 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2},
 { 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2},
 { 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2},
 { 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2},
 { 2, 2, 2, 2, 5, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2},
 { 2, 2, 2, 5, 5, 5, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2},
 { 2, 2, 5, 5, 5, 5, 5, 2, 2, 2, 2, 2, 2, 2, 2, 2},
 { 2, 2, 2, 5, 5, 5, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2},
 { 2, 2, 2, 2, 5, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2},
 { 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2},
 { 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2},
 { 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2},
 { 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2},
 { 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2},
 { 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2},
 { 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2}
};

map_type ceiling={
 { 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9},
 { 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9},
 { 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9},
 { 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9},
 { 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9},
 { 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9},
 { 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9},
 { 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9},
 { 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9},
 { 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9},
 { 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9},
 { 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9},
 { 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9},
 { 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9},
 { 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9},
 { 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9}
};

pcx_struct textmaps,bg;  // PCX files for tiles and background
FILE *handle;            // Handle for lightsourcing file
unsigned char litetable[MAXLIGHT+1][PALETTESIZE];
unsigned char *litelevel;
unsigned char *texturelist[15];

long viewing_angle=13;
long intensity=MAXLIGHT;
long viewer_height=32;
long ambient_level=MAXLIGHT;
long xview=8*64+32;
long yview=8*64+32;

HTimer timer1;                 // instance of HTimer object
long elapsed_time;
unsigned long frame[MAXFRAMES];
int frameptr=0;
int framewrap=0;

void main(int argc,char* argv[])
{
	event_struct events;
  unsigned far char *screen_buffer;

  // Initialize event selector:

  int which_events = WHICH_EVENTS;

  // Read arguments from command line if present:

  if (argc>=2) intensity=atof(argv[1]);
  if (argc>=3) ambient_level=atof(argv[2]);

  // Load texture map images:

  if (loadPCX("walls.pcx",&textmaps)) exit(1);

  // Load background screen image:

  if (loadPCX("demobg8.pcx",&bg)) exit(1);

  // Create texture list array:

  for (unsigned int i=0; i<NUM_IMAGES; i++) {
    texturelist[i]=&(textmaps.image[(i/5)*320*64+i%5*64]);
  }

  // Load lightsourcing tables:

  if ((handle=fopen("litesorc.dat","rb"))==NULL) {
    perror("Error");
    exit;
  }
  fread(litetable,MAXLIGHT+1,PALETTESIZE,handle);
  fclose(handle);

  // Initialize array of light levels:

  litelevel=new unsigned char[MAXDISTANCE];

  // Calculate light intensities for all possible
  // distances:

  for (int distance=1; distance<MAXDISTANCE; distance++) {
    float ratio=(float)intensity/distance*MULTIPLIER;
    if (ratio>1.0) ratio=1.0;
    litelevel[distance]=ratio*MAXLIGHT;
  }

  // Point variable at video memory:

  char far *screen=(char far *)MK_FP(0xa000,0);

	// Create offscreen video buffer:

	if ((screen_buffer=new unsigned char [64000])==NULL) {;
    perror("Error");
    exit;
  }

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

  // Set mode 13h:

  setmode(0x13);

  // Set the palette:

  setpalette(textmaps.palette);

  // Clear the screen:

  cls(screen_buffer);

  // Copy background into screen buffer:

  for (i=0; i<64000; i++)
    screen_buffer[i]=bg.image[i];

  // Draw initial view of maze:

  draw_maze(walls,flor,ceiling,screen_buffer,xview,yview,
            viewing_angle,viewer_height,ambient_level,
            textmaps.image,litetable,litelevel);

  // Put screen buffer contents on screen:

  putwindow(0,0,320,200,screen_buffer);

  events.quit_game=0;

  timer1.timerOn();

	// Let's go for a walk in the maze:

	while(!events.quit_game) {

		// Draw the maze in screen buffer:

    clrwin(VIEWPORT_LEFT,VIEWPORT_TOP,VIEWPORT_WIDTH,
             VIEWPORT_HEIGHT,screen_buffer);

    elapsed_time = timer1.timerOff();
    AddFrameTime(elapsed_time);
    timer1.timerOn();

    // Put first frame into buffer:

    draw_maze(walls,flor,ceiling,screen_buffer,xview,yview,viewing_angle,
              viewer_height,ambient_level,textmaps.image,
              litetable,litelevel);

		// Move screen buffer to screen:

    putwindow(VIEWPORT_LEFT,VIEWPORT_TOP,VIEWPORT_WIDTH,
             VIEWPORT_HEIGHT,screen_buffer);

		// Move viewer according to input events:

		getevent(which_events,&events);

    // Did a go forward event occur?

    if (events.go_forward) {

      // If so, assume viewer is standing at origin of map...

      long new_x=0;

      // ...facing north. Calculate destination by checking
      // timer to see what fraction of a second has passed
      // since last frame of animation:

      long new_y=(long)(DISTANCE_PER_SECOND/1000000.0
                   *elapsed_time)<<16;

      // Rotate destination to actual viewer heading and
      // translate relative to actual viewer position:

      xview+=(fixmul(new_x,COS(viewing_angle))
              -fixmul(new_y,SIN(viewing_angle)))>>16;
      yview+=(fixmul(new_y,COS(viewing_angle))
              +fixmul(new_x,SIN(viewing_angle)))>>16;
    }

    // Did a go back event occur?

    if (events.go_back) {

      // Repeat procedure for forward event...

      long new_x=0;

      // ...but make the distance negative, to the rear
      // of the viewer:

      long new_y=-(long)(DISTANCE_PER_SECOND/1000000.0
                             *elapsed_time)<<16;

      // Rotate and translate destination as above:

      xview+=(fixmul(new_x,COS(viewing_angle))
              -fixmul(new_y,SIN(viewing_angle)))>>16;
      yview+=(fixmul(new_y,COS(viewing_angle))
              +fixmul(new_x,SIN(viewing_angle)))>>16;
    }

    // Did a go right event occur?

    if (events.go_right) {

      // If so, calculate new angle based on elapsed time since
      // last frame of animation:

      viewing_angle+=((float)ROTATION_PER_SECOND/1000000.0
                         *elapsed_time);

      // Wrap around to 0 if angle exceeds maximum degrees:

      if (viewing_angle>NUMBER_OF_DEGREES-1)
           viewing_angle-=NUMBER_OF_DEGREES;
    }

    // Did a go left event occur?

    if (events.go_left) {

      // If so, calculate new angle based on elapsed time since
      // last frame of animation:

      viewing_angle-=((float)ROTATION_PER_SECOND/1000000.0
                         *elapsed_time);

      // Wrap around to maximum degree if angle less than zero:

      if (viewing_angle<0) viewing_angle+=NUMBER_OF_DEGREES;
    }
  }

  // Reset video mode and exit:

  end_events();
  setmode(oldmode);
  PrintFrameRate();

  //
  // Code added to deallocate memory:
  //

  if(litelevel) delete [] litelevel;
  if( bg.image) delete bg.image;
  if( textmaps.image) delete textmaps.image;
  delete screen_buffer;
}

void PrintFrameRate()
{
 int numframes;
 unsigned long looptime = 0;

 if (framewrap) numframes = MAXFRAMES;
 else numframes = frameptr;
 cprintf("Average time per frame (ms):");
 if (numframes) {
   for (int i = 0; i < numframes; i++) looptime += frame[i];
   looptime /= numframes;
   cprintf(" %i\r\n", looptime);
   printf("Average frames per second: %d",1000/looptime);
 }
 else cprintf(" timer disabled\r\n");
}

void near AddFrameTime(long elapsedtime)
{
  frame[frameptr] = (elapsedtime/1000);
  frameptr++;
  if (frameptr == MAXFRAMES) {
    frameptr = 0;
    framewrap = -1;
  }
}
