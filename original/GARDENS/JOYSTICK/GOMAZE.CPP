
// GOMAZE.CPP
//
// Uses event manager for an animated tour of a
// wireframe maze.
//
// Written by Christopher Lampton for
// GARDENS OF IMAGINATION (Waite Group Press)

#include  <stdio.h>
#include	<dos.h>
#include	<conio.h>
#include  <stdlib.h>
#include  <time.h>
#include  "screen.h"
#include  "bresn.h"
#include  "evntmgr1.h"

#define   FRAMES_PER_SECOND 6
#define   WHICH_EVENTS KEYBOARD_EVENTS+MOUSE_EVENTS+JOYSTICK_EVENTS

char maze[16][16]={
  {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,1,1,1,0,1,0,1,0,1,1,0,1},
  {1,0,0,0,1,0,0,0,1,1,1,0,1,1,0,1},
  {1,0,0,0,0,0,1,0,0,0,1,0,0,1,0,1},
  {1,0,1,1,1,0,1,0,1,0,1,1,1,1,0,1},
  {1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,1},
  {1,0,1,1,1,0,1,0,1,1,1,0,1,1,0,1},
  {1,0,1,0,0,0,1,1,0,0,1,0,1,1,0,1},
  {1,0,1,0,1,0,1,1,1,1,1,0,1,1,0,1},
  {1,0,1,0,1,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,1,1,1,0,1,0,1,0,1,1,1,1,0,1},
  {1,0,0,0,1,0,1,0,1,0,0,0,0,1,0,1},
  {1,0,1,1,1,0,1,1,1,1,1,1,1,1,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};

typedef struct xy {
  int x,y;
};

struct xy increment[4]={{-1,0},{0,1},{1,0},{0,-1}};
struct xy left[4]={{0,-1},{-1,0},{0,1},{1,0}};
struct xy right[4]={{0,1},{1,0},{0,-1},{-1,0}};
struct xy pos={5,5};

int direction=0;
int visibility=4;
long ticks_per_frame;

void drawmaze(char *);
void drawbox(char *);

void main(void)
{

  int scan;
  event_struct events;
  struct xy newpos;

  // Initialize event selector:

  int which_events = WHICH_EVENTS;

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

  // Create pointer to video memory:

  char far *screen=(char far *)MK_FP(0xa000,0);

  // Create offscreen video buffer:

  char far *screen_buffer=new unsigned char [64000];

  // Save previous video mode:

	int oldmode=*(int *)MK_FP(0x40,0x49);

  // Put display in mode 13h:

	setmode(0x13);

  // Clear display:

  cls(screen_buffer);

  // Draw window on display:

  drawbox(screen_buffer);

  // Set number of ticks per frame:

  ticks_per_frame = CLK_TCK / FRAMES_PER_SECOND;

  // Initialize the frame timer:

  clock_t lastframe=clock();

  // Make sure we get at least one frame
  // into the maze:

  events.quit_game=0;

  // Let's go for a walk in the maze:

  while(!events.quit_game) {

    // Draw the maze in screen buffer:

    drawmaze(screen_buffer);

    // Move screen buffer to screen:

    putwindow(0,0,320,200,screen_buffer);

    // Pause until time for next frame:

    while ((clock()-lastframe)<ticks_per_frame);

    // Start timing another frame:

    lastframe=clock();

    // Check for input events:

    getevent(which_events,&events);

    // Move viewer according to input events:

    // Do we want to move forward?

    if (events.go_forward) {
      newpos.x=pos.x+increment[direction].x;
      newpos.y=pos.y+increment[direction].y;
      if (!maze[newpos.x][newpos.y]) {
         pos.x=newpos.x;
         pos.y=newpos.y;
      }
    }

    // ...or do we want to move backward?

    else if (events.go_back) {
      newpos.x=pos.x-increment[direction].x;
      newpos.y=pos.y-increment[direction].y;
      if (!maze[newpos.x][newpos.y]) {
         pos.x=newpos.x;
         pos.y=newpos.y;
      }
    }

    // Do we want to turn left?

    if (events.go_left) {
      --direction;
      if (direction<0) direction=3;
    }

    // ...or do we want to turn right?

    else if (events.go_right) {
      direction++;
      if (direction>3) direction=0;
    }
  }

  // Terminate event manager:

  end_events();

  // Release memory
  delete screen_buffer;

  // Restore old video mode:

  setmode(oldmode);
}

void drawbox(char *screen)

// Draw box around maze window:

{
  linedraw(82,19,294,19,15,screen);
  linedraw(294,19,294,119,15,screen);
  linedraw(294,119,82,119,15,screen);
  linedraw(82,119,82,19,15,screen);
}

void drawmaze(char *screen)

// Draw the maze stored in array maze[] into video
// memory at address screen.

{
  struct xy block,lblock,rblock;
  int oldleft,oldright;

  // Clear the maze window:

  clrwin(83,20,211,99,screen);

  // Draw the maze at each distance allowed by visibility

  for(int dist=0; dist<visibility; dist++) {

    // Find current square of maze:

    block.x=pos.x+dist*increment[direction].x;
    block.y=pos.y+dist*increment[direction].y;

    // Find square to the left of current square:

    lblock.x=block.x+left[direction].x;
    lblock.y=block.y+left[direction].y;

    // Find square to the right of current square:

    rblock.x=block.x+right[direction].x;
    rblock.y=block.y+right[direction].y;

    // Draw image of squares according to distance:

    switch(dist) {
      case 0:  // Draw current square

        // Is wall open to the left?
        // If not, draw wall

        if (maze[lblock.x][lblock.y]) {
          linedraw(82,19,135,44,15,screen);
          linedraw(135,44,135,93,15,screen);
          linedraw(135,93,82,118,15,screen);
        }
        else { // Else draw opening
          if (maze[lblock.x+increment[direction].x]
                  [lblock.y+increment[direction].y]) {
            linedraw(82,44,135,44,15,screen);
            linedraw(135,44,135,93,15,screen);
            linedraw(135,93,82,93,15,screen);
          }
        }

        // Is wall open to the right?
        // If not, draw wall

        if (maze[rblock.x][rblock.y]) {
          linedraw(294,19,242,44,15,screen);
          linedraw(242,44,242,93,15,screen);
          linedraw(294,118,242,93,15,screen);
        }
        else { // Else draw opening
          if (maze[rblock.x+increment[direction].x]
                  [rblock.y+increment[direction].y]) {
            linedraw(294,44,242,44,15,screen);
            linedraw(242,44,242,93,15,screen);
            linedraw(242,93,294,93,15,screen);
          }
        }
        break;
      case 1: // Repeat for next square

        // Can we see through the next square?
        // If not, draw wall

        if (maze[block.x][block.y]) {
          linedraw(135,44,135,93,15,screen);
          linedraw(242,44,242,93,15,screen);
          linedraw(135,44,242,44,15,screen);
          linedraw(135,93,242,93,15,screen);
        }
        else { // Else draw sides of next square
          if (maze[lblock.x][lblock.y]) {
            linedraw(135,44,162,57,15,screen);
            linedraw(162,57,162,80,15,screen);
            linedraw(162,80,135,93,15,screen);
          }
          else {
            if (maze[lblock.x+increment[direction].x]
                    [lblock.y+increment[direction].y]) {
              linedraw(135,57,162,57,15,screen);
              linedraw(162,57,162,80,15,screen);
              linedraw(162,80,135,80,15,screen);
            }
          }
          if (maze[rblock.x][rblock.y]) {
            linedraw(242,44,215,57,15,screen);
            linedraw(215,57,215,80,15,screen);
            linedraw(215,80,242,93,15,screen);
          }
          else {
            if (maze[rblock.x+increment[direction].x]
                    [rblock.y+increment[direction].y]) {
              linedraw(242,57,215,57,15,screen);
              linedraw(215,57,215,80,15,screen);
              linedraw(215,80,242,80,15,screen);
            }
          }
        }
        break;
      case 2: // Do it again
        if (maze[block.x][block.y]) {
          linedraw(162,57,162,80,15,screen);
          linedraw(215,57,215,80,15,screen);
          linedraw(162,57,215,57,15,screen);
          linedraw(162,80,215,80,15,screen);
        }
        else {
          if (maze[lblock.x][lblock.y]) {
            linedraw(162,57,175,63,15,screen);
            linedraw(175,63,175,74,15,screen);
            linedraw(175,74,162,80,15,screen);
          }
          else {
            if (maze[lblock.x+increment[direction].x]
                    [lblock.y+increment[direction].y]) {
              linedraw(162,63,175,63,15,screen);
              linedraw(175,63,175,74,15,screen);
              linedraw(175,74,162,74,15,screen);
            }
          }
          if (maze[rblock.x][rblock.y]) {
            linedraw(215,57,202,63,15,screen);
            linedraw(202,63,202,74,15,screen);
            linedraw(202,74,215,80,15,screen);
          }
          else {
            if (maze[rblock.x+increment[direction].x]
                    [rblock.y+increment[direction].y]) {
              linedraw(215,63,202,63,15,screen);
              linedraw(202,63,202,74,15,screen);
              linedraw(202,74,215,74,15,screen);
            }
          }
        }
        break;
      case 3: // And again
        if (maze[block.x][block.y]) {
          linedraw(175,63,175,74,15,screen);
          linedraw(202,63,202,74,15,screen);
          linedraw(175,63,202,63,15,screen);
          linedraw(175,74,202,74,15,screen);
        }
        else {
          if (maze[lblock.x][lblock.y]) {
            linedraw(175,63,182,66,15,screen);
            linedraw(182,66,182,70,15,screen);
            linedraw(182,70,175,74,15,screen);
          }
          else {
            if (maze[lblock.x+increment[direction].x]
                    [lblock.y+increment[direction].y]) {
              linedraw(175,66,182,66,15,screen);
              linedraw(182,66,182,70,15,screen);
              linedraw(182,70,175,70,15,screen);
            }
          }
          if (maze[rblock.x][rblock.y]) {
            linedraw(202,63,195,66,15,screen);
            linedraw(195,66,195,70,15,screen);
            linedraw(195,70,202,74,15,screen);
          }
          else {
            if (maze[rblock.x+increment[direction].x]
                    [rblock.y+increment[direction].y]) {
              linedraw(202,66,195,66,15,screen);
              linedraw(195,66,195,70,15,screen);
              linedraw(195,70,202,70,15,screen);
            }
          }
        }
        break;
    }

    // If view is obscured by wall, stop drawing:

    if (maze[block.x][block.y]) break;
  }
}
