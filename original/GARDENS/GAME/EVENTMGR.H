// EVENTMNGR.H
// Written July 29,1993 by Christopher Lampton
//  for GARDENS OF IMAGINATION (Waite Group Press)

// Definitions, data types and prototypes for
// the event management function in module
// EVNTMNGR.CPP

// Key definitions by scan code:

#define   FORWARDKEY 72
#define   BACKKEY    80
#define   LEFTKEY    75
#define   RIGHTKEY   77
#define   QUITKEY     1

// Definitions for requesting events:

const	MOUSE_EVENTS=1,JOYSTICK_EVENTS=2,KEYBOARD_EVENTS=4;

// Structure for passing events to calling program:

struct event_struct {
  int go_forward,go_back,go_left,go_right,abort;
};

void init_events();	// Initialize event manager
void end_events();  // Terminate event manager
void setmin();			// Set minimum joystick callibrations
void setmax();			// Set maximum joystick callibrations
void setcenter();		// Set center joystick callibrations
int joystick_present(); // Detect presence of joystick
void getevent(int,event_struct *);	// Get events from selected devices
