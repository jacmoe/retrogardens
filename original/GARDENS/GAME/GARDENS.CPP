
// GARDENS.CPP
//
// Main module for Gardens of Imagination game
//
// Written by Christopher Lampton for
// Gardens of Imagination (Waite Group Press,1994)

#include  <stdio.h>
#include	<dos.h>
#include	<conio.h>
#include  <stdlib.h>
#include  <math.h>
#include  "screen.h"
#include  "gardens.h"
#include	"pcx.h"
#include  "objdraw.h"
#include  "optray.h"
#include  "eventmgr.h"
#include  "bitmap.h"
#include  "io.h"
#include  "trig.h"
#include  "types.h"
#include  "htimer.h"
#include  "fixpoint.h"
#include  "palfade.h"

// Function prototypes:

void PrintFrameRate();
void near AddFrameTime(long elapsedtime);
void drawmap(char far *screen);
void drawplayer(char far *screen);
void eraseplayer(char far *screen);

// Constant definitions:

const MAXFRAMES=500;
const NUM_IMAGES=15;
const float MULTIPLIER=3;
const DISTANCE_PER_SECOND=128;
const ROTATION_PER_SECOND=1024;
const WHICH_EVENTS=KEYBOARD_EVENTS+MOUSE_EVENTS;
const MAPX=240;
const MAPY=28;
const PLAYER_COLOR=13;
const WALL_COLOR=15;
const long TITLE_DELAY=2000000L;

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

map_type objmap={
 { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
 { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
 { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
 { 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
 { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
 { 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
 { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
 { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
 { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
 { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
 { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
 { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
 { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
 { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
 { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
 { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
};

objtype obj[]={
  {0,0,5*64+32,5*64+32,0,0,0,0,64,64,64,64,0,0,0},   // Skull
  {0,0,3*64+32,3*64+32,0,0,0,0,64,64,64,64,0,0,3},   // Column
};
char far *image[NUMIMAGES];

pcx_struct textmaps,logo,bg,objs;  // PCX files for tiles and background
FILE *handle;            // Handle for lightsourcing file
unsigned char litetable[MAXLIGHT+1][PALETTESIZE];
unsigned char *litelevel;
unsigned char *texturelist[15];
unsigned char wallbuffer[64*64];

long viewing_angle=130;
//long viewing_angle=0;
long intensity=MAXLIGHT;
long viewer_height=32;
// long ambient_level=MAXLIGHT;
long ambient_level=10;
long xview=6*64+32;
long yview=6*64+32;
long nextx;
long nexty;

HTimer timer1;                 // instance of HTimer object
HTimer* timer2;                // pointer to HTimer object
long elapsed_time;
unsigned long frame[MAXFRAMES];
int frameptr=0;
int framewrap=0;

void main(int argc,char* argv[])
{
	event_struct events;
  unsigned far char *screen_buffer;

  // Read arguments from command line if present:

  if (argc>=2) intensity=atof(argv[1]);
  if (argc>=3) ambient_level=atof(argv[2]);

  // Save previous video mode:

  int oldmode=*(int *)MK_FP(0x40,0x49);

  // Set mode 13h:

  setmode(0x13);

  // Point variable at video memory:

  char far *screen=(char far *)MK_FP(0xa000,0);

	// Create offscreen video buffer:

	if ((screen_buffer=new unsigned char [64000])==NULL) {;
    perror("Error");
    exit;
  }

  if (loadPCX("objs.pcx",&objs)) exit(1);

  for (unsigned int i=0; i<NUMIMAGES; i++)
  {
    if ((image[i]=new unsigned char[4096])==NULL)
        exit(1);
    for (unsigned int j=0; j<64; j++) {
      for (unsigned int k=0; k<64; k++) {
        image[i][k*64+j]=objs.image[((i/5)*320*64+(i%5)
                                         *64+j+k*320)];
      }
    }
  }

  delete(logo.image);

  // Allocate memory for timer object:

  if ((timer2 = new HTimer()) == NULL) {
    perror("Initialization of timer object failed");
    exit;
  }

  int abort_titles=0;

  fadepalout(0,256);

  // Load Waite Group logo:

  if (loadPCX("waite.pcx",&logo)) exit(1);

  // Copy logo into video memory:

  for (i=0; i<64000; i++)
    screen[i]=logo.image[i];

  // Fade logo in:

  fadepalin(0,256,logo.palette);

  // Start timer running:

  timer1.timerOn();

  elapsed_time = timer1.getElapsed();

  while(elapsed_time<TITLE_DELAY) {
    if (kbhit()) {
      abort_titles=-1;
      break;
    }
    elapsed_time = timer1.getElapsed();
  }

  fadepalout(0,256);
  cls(screen);

  delete(logo.image);

  timer1.timerOff();

  fadepalout(0,256);

  if (!abort_titles) {

    // Load mountain logo:

    if (loadPCX("chrispro.pcx",&logo)) exit(1);

    // Copy logo into video memory:

    for (i=0; i<64000; i++)
      screen[i]=logo.image[i];

    // Fade logo in:

    fadepalin(0,256,logo.palette);

    // Start timer running:

    timer1.timerOn();

    elapsed_time = timer1.getElapsed();

    while(elapsed_time<TITLE_DELAY) {
      if (kbhit()) {
        abort_titles=-1;
        break;
      }
      elapsed_time = timer1.getElapsed();
    }

    fadepalout(0,256);
    cls(screen);

    delete(logo.image);

    timer1.timerOff();

    fadepalout(0,256);
  }

  if (!abort_titles) {

    // Load title screen:

    if (loadPCX("title.pcx",&logo)) exit(1);

    // Copy logo into video memory:

    for (i=0; i<64000; i++)
      screen[i]=logo.image[i];

    // Fade logo in:

    fadepalin(0,256,logo.palette);

    // Start timer running:

    timer1.timerOn();

    elapsed_time = timer1.getElapsed();

    while(elapsed_time<TITLE_DELAY) {
      if (kbhit()) {
        abort_titles=-1;
        break;
      }
      elapsed_time = timer1.getElapsed();
    }

    fadepalout(0,256);
    cls(screen);

    delete(logo.image);

    timer1.timerOff();
  }

  // Load texture map images:

  if (loadPCX("walls.pcx",&textmaps)) exit(1);
  for (i=0; i<15; i++) {
    for (int y=0; y<64; y++) {
      for (int x=0; x<64; x++) {
        wallbuffer[y*64+x] = textmaps.image[i/5*320*64+i%5*64+y*320+x];
      }
    }
    for (y=0; y<64; y++) {
      for (int x=0; x<64; x++) {
        textmaps.image[i/5*320*64+i%5*64+y*320+x]=wallbuffer[x*64+y];
      }
    }
  }

  // Load background screen image:

  if (loadPCX("screen.pcx",&bg)) exit(1);

  // Create texture list array:

  for (i=0; i<NUM_IMAGES; i++) {
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

	// Initialize event manager:

	init_events();

	// Calibrate the user's joystick:

	if (WHICH_EVENTS & JOYSTICK_EVENTS) {
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

  // Clear the screen:

  cls(screen_buffer);

  // Copy background into screen buffer:

  for (i=0; i<64000; i++)
    screen_buffer[i]=bg.image[i];

  // Draw initial view of maze:

  draw_maze(walls,flor,ceiling,screen_buffer,xview,yview,viewing_angle,
            viewer_height,ambient_level,textmaps.image,
            litetable,litelevel);

  // Draw automap:

  drawmap(screen_buffer);

  // Put player in the automap:

  drawplayer(screen_buffer);

  // Put screen buffer contents on screen:

  putwindow(0,0,320,200,screen_buffer);

  // Set the palette:

  fadepalin(0,256,textmaps.palette);

  events.abort=0;

  timer1.timerOn();

	// Let's go for a walk in the maze:

	while(!events.abort) {

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

		getevent(WHICH_EVENTS,&events);

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

      nextx=xview+(fixmul(new_x,COS(viewing_angle))
              -fixmul(new_y,SIN(viewing_angle))>>16);
      nexty=yview+(fixmul(new_y,COS(viewing_angle))
              +fixmul(new_x,SIN(viewing_angle))>>16);
      if ((walls[nextx/64][nexty/64]==0)
         &(objmap[nextx/64][nexty/64]==0)) {
        eraseplayer(screen);
        xview=nextx;
        yview=nexty;
        drawplayer(screen);
      }
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

      nextx=xview+(fixmul(new_x,COS(viewing_angle))
              -fixmul(new_y,SIN(viewing_angle))>>16);
      nexty=yview+(fixmul(new_y,COS(viewing_angle))
              +fixmul(new_x,SIN(viewing_angle))>>16);
      if ((walls[nextx/64][nexty/64]==0)
         &(objmap[nextx/64][nexty/64]==0)) {
        eraseplayer(screen);
        xview=nextx;
        yview=nexty;
        drawplayer(screen);
      }
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

  // Release memory:

    if(litelevel) delete [] litelevel;
    if( bg.image) delete bg.image;
    if( textmaps.image) delete textmaps.image;
    delete timer2;
    for (int im=0; im<NUMIMAGES; im++) {
      if (image[im]) delete [] image[im];
    }
    if( objs.image) delete objs.image;
    delete screen_buffer;

  // Reset video mode and exit:

  setmode(oldmode);
  PrintFrameRate();
  end_events();
}

void drawmap(char far *screen)
// Draw automap
{
  unsigned int ptr=MAPY*320+MAPX;
  for (int x=0; x<16; x++) {
    for (int y=0; y<16; y++) {
      if (walls[x][y]) {
        screen[ptr]=WALL_COLOR;
        screen[ptr+1]=WALL_COLOR;
        screen[ptr+2]=WALL_COLOR;
        screen[ptr+3]=WALL_COLOR;
        screen[ptr+320]=WALL_COLOR;
        screen[ptr+321]=WALL_COLOR;
        screen[ptr+322]=WALL_COLOR;
        screen[ptr+323]=WALL_COLOR;
        screen[ptr+640]=WALL_COLOR;
        screen[ptr+641]=WALL_COLOR;
        screen[ptr+642]=WALL_COLOR;
        screen[ptr+643]=WALL_COLOR;
        screen[ptr+960]=WALL_COLOR;
        screen[ptr+961]=WALL_COLOR;
        screen[ptr+962]=WALL_COLOR;
        screen[ptr+963]=WALL_COLOR;
      }
      ptr+=4;
    }
    ptr+=4*320-64;
  }
}

void drawplayer(char far *screen)
// Put player character in automap:
{
  unsigned int ptr=MAPY*320+xview/64*1280+MAPX+yview/64*4;
  screen[ptr]=PLAYER_COLOR;
  screen[ptr+1]=PLAYER_COLOR;
  screen[ptr+2]=PLAYER_COLOR;
  screen[ptr+3]=PLAYER_COLOR;
  screen[ptr+320]=PLAYER_COLOR;
  screen[ptr+321]=PLAYER_COLOR;
  screen[ptr+322]=PLAYER_COLOR;
  screen[ptr+323]=PLAYER_COLOR;
  screen[ptr+640]=PLAYER_COLOR;
  screen[ptr+641]=PLAYER_COLOR;
  screen[ptr+642]=PLAYER_COLOR;
  screen[ptr+643]=PLAYER_COLOR;
  screen[ptr+960]=PLAYER_COLOR;
  screen[ptr+961]=PLAYER_COLOR;
  screen[ptr+962]=PLAYER_COLOR;
  screen[ptr+963]=PLAYER_COLOR;
}

void eraseplayer(char far *screen)
// Delete player character from automap:
{
  unsigned int ptr=MAPY*320+xview/64*1280+MAPX+yview/64*4;
  screen[ptr]=0;
  screen[ptr+1]=0;
  screen[ptr+2]=0;
  screen[ptr+3]=0;
  screen[ptr+320]=0;
  screen[ptr+321]=0;
  screen[ptr+322]=0;
  screen[ptr+323]=0;
  screen[ptr+640]=0;
  screen[ptr+641]=0;
  screen[ptr+642]=0;
  screen[ptr+643]=0;
  screen[ptr+960]=0;
  screen[ptr+961]=0;
  screen[ptr+962]=0;
  screen[ptr+963]=0;
}

void PrintFrameRate()
// Print frame rate to text screen:
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
// Add time of last frame to frame time queue:
{
  frame[frameptr] = (elapsedtime/1000);
  frameptr++;
  if (frameptr == MAXFRAMES) {
    frameptr = 0;
    framewrap = -1;
  }
}
