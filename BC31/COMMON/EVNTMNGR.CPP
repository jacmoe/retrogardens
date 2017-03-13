
// EVNTMNGR.CPP
//
// A set of high-level functions to monitor input from
// the keyboard, the joystick and the mouse.
//
// Copyright 1993 by Christopher Lampton and The
// Waite Group Press

#include	<stdio.h>
#include	"jmio.h"
#include	"evntmngr.h"

// Variable declarations

int x,y;	// All purpose coordinate variables
int xmin,xmax,xcent,ymin,ymax,ycent; // Joystick cali-
                                     // bration variables
int lastkey=0,keycount=0;	// Keyboard variables

void init_events()
// Initialize event manager
{
	initmouse();	// Initialize the mouse driver
	rempointer();	// Remove mouse pointer from screen
}

void setmin()
// Set minimum joystick coordinates
{
  while (!readjbutton(JBUTTON1)); // Loop until joystick
                                  //  button pressed
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
  xmax=readstick(JOY_X);          // Get x coordinate
  ymax=readstick(JOY_Y);          // Get y coordinate
  while (readjbutton(JBUTTON1));  // Loop until button
                                  //  released
}

void setcenter()
// Set center joystick coordinates
{
  while (!readjbutton(JBUTTON1)); // Loop until joystick
                                  //  button pressed
  xcent=readstick(JOY_X);         // Get x coordinate
  ycent=readstick(JOY_Y);         // Get y coordinate
  while (readjbutton(JBUTTON1));  // Loop until button
                                  //  released
}

int getevent(int event_mask)
// Get events from devices selected by EVENT_MASK
{
  int event_return=NOEVENT;				// Initialize events to
                                  //  NO EVENTS
  // If joystick events requested....
  if (event_mask & JOYSTICK_EVENTS) {
    // ...set left, right, up, down and button events:
    if (readstick(JOY_X)<(xcent-4)) event_return|=LEFT;
    if (readstick(JOY_X)>(xcent+10)) event_return|=RIGHT;
    if (readstick(JOY_Y)<(xcent-4)) event_return|=UP;
    if (readstick(JOY_Y)>(xcent+10)) event_return|=DOWN;
    if (readjbutton(JBUTTON1)) event_return|=LBUTTON;
    if (readjbutton(JBUTTON2)) event_return|=RBUTTON;
	}
	// If mouse events requested....
	if (event_mask & MOUSE_EVENTS) {
		// ...set left, right, up, down and button events:
		relpos(&x,&y);	// Read relative mouse position
		if (x<0) event_return|=LEFT;
    if (x>0) event_return|=RIGHT;
    if (y<0) event_return|=UP;
    if (y>0) event_return|=DOWN;
    int b=readmbutton();	// Read mouse button
    if (b&MBUTTON1) event_return|=LBUTTON;
    if (b&MBUTTON2) event_return|=RBUTTON;
	}
  // If keyboard events requested
  if (event_mask & KEYBOARD_EVENTS) {
    // ...set left, right, up, down and "button" events:
    int k=scankey();  // Read scan code of last key pressed
    if (k==0) {       // If no key pressed
      if (lastkey) {  // Set to last active key
        k=lastkey;
        --keycount;   // Check repeat count
        if (keycount==0) lastkey=0;	// If over, deactivate
                                    //   key
			}
		}
		else {	// If key pressed...
			lastkey=k;	// ...note which key
			keycount=20;	// ...set repeat count
		}
    // ...and determine which key event, if any, occurred:
    switch (k) {
      case ENTER: event_return|=LBUTTON; break;
      case TABKEY: event_return|=RBUTTON; break;
      case UP_ARROW: event_return|=UP; break;
      case DOWN_ARROW: event_return|=DOWN; break;
      case LEFT_ARROW: event_return|=LEFT; break;
      case RIGHT_ARROW: event_return|=RIGHT; break;
    }
  }
  return(event_return);
}
