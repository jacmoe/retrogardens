
// EVNTMGR1.CPP
//
// A set of high-level functions to monitor input from
// the keyboard, the joystick and the mouse. The getevent()
// function is specific to the events required by the
// WIREMAZE.CPP and BITMAZE.CPP programs.
//
// Written July 30, 1993 by Christopher Lampton
//  for GARDENS OF IMAGINATION (Waite Group Press)

#include	<stdio.h>
#include  <dos.h>
#include	"io.h"
#include	"evntmgr1.h"

// Constant definition:

const GAMEPORT=0x0201;

// Variable declarations

int x,y;	// All purpose coordinate variables
int xmin,xmax,xcent,ymin,ymax,ycent; // Joystick cali-
                                     // bration variables
char far keybuffer[256]; // Buffer for 256 scan codes
int lastkey=0,keycount=0;	// Keyboard variables

void init_events()
// Initialize event manager
{
  initkey(keybuffer); // Install alternate keyboard driver
	initmouse();	      // Initialize the mouse driver
	rempointer();	      // Remove mouse pointer from screen
}

void setmin()
// Set minimum joystick coordinates
{
  while (!readjbutton(JBUTTON1)); // Loop until joystick
                                  //  button pressed
  for (long i=1; i<100000; i++);  // Brief delay
  xmin=readstick(JOY_X);          // Get x coordinate
  ymin=readstick(JOY_Y);          // Get y coordinate
  while (readjbutton(JBUTTON1));  // Loop until button
                                  //  released
}

void setmax()
// Set maximum joystick coordinates
{
  while (!readjbutton(JBUTTON1)); // Loop until joystick
                                  //  button pressed
  for (long i=1; i<100000; i++);  // Brief delay
  xmax=readstick(JOY_X);          // Get x coordinate
  ymax=readstick(JOY_Y);          // Get y coordinate
  while (readjbutton(JBUTTON1));  // Loop until button
                                  //  released
}

int joystick_present()

// Function to detect presence of joystick, as follows:
//
// 0 - no stick
// 1 - stick 1 present
// 2 - stick 2 present
// 3 - both sticks present
//
// Adapted from code written by Mark Betz for
// Flights of Fantasy (Waite Group Press, 1993)

{
  int mask=0;

  // Joystick 1 present?

  outportb(GAMEPORT,0);
  for(unsigned int i=0; i<65535; i++)
    if ((inportb(GAMEPORT) & 1) == 0) break;
  if (i !=65535) mask = mask | 1;

  // Joystick 2 present?

  outportb(GAMEPORT,0);
  for(i=0; i<65535; i++)
    if ((inportb(GAMEPORT) & 2) == 0) break;
  if (i !=65535) mask = mask | 2;

  return (mask);
}

void end_events()
// Terminate event manager routines
{
  remkey();
}

void setcenter()
// Set center joystick coordinates
{
  while (!readjbutton(JBUTTON1)); // Loop until joystick
                                  //  button pressed
  for (long i=1; i<100000; i++);  // Brief delay
  xcent=readstick(JOY_X);         // Get x coordinate
  ycent=readstick(JOY_Y);         // Get y coordinate
  while (readjbutton(JBUTTON1));  // Loop until button
                                  //  released
}

void getevent(int event_mask,event_struct *events)
// Get events from devices selected by EVENT_MASK
{

  // Clear any events in structure:

  events->go_forward=0;
  events->go_back=0;
  events->go_left=0;
  events->go_right=0;
  events->quit_game=0;

  // If joystick events requested....
  if (event_mask & JOYSTICK_EVENTS) {
    if (readstick(JOY_Y)<(ycent-100)) events->go_forward=1;
    if (readstick(JOY_Y)>(ycent+100)) events->go_back=1;
    if (readstick(JOY_X)<(xcent-100)) events->go_left=1;
    if (readstick(JOY_X)>(xcent+100)) events->go_right=1;
    if (readjbutton(JBUTTON1)) events->quit_game=1;
	}
	// If mouse events requested....
	if (event_mask & MOUSE_EVENTS) {
		relpos(&x,&y);	// Read relative mouse position
    if (y<-5) events->go_forward=1;
    if (y>5) events->go_back=1;
		if (x<-20) events->go_left=1;
    if (x>20) events->go_right=1;
    int b=readmbutton();	// Read mouse button
    if (b&LMBUTTON) events->quit_game=1;
	}
  // If keyboard events requested....
  if (event_mask & KEYBOARD_EVENTS) {
    if (keybuffer[FORWARDKEY]) events->go_forward=1;
    if (keybuffer[BACKKEY]) events->go_back=1;
    if (keybuffer[LEFTKEY]) events->go_left=1;
    if (keybuffer[RIGHTKEY]) events->go_right=1;
    if (keybuffer[QUITKEY]) events->quit_game=1;
  }
}
