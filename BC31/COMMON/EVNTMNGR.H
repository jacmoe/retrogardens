
const NOEVENT=0,LBUTTON=1,RBUTTON=2,UP=4,DOWN=8,LEFT=16,RIGHT=32;
const	MOUSE_EVENTS=1,JOYSTICK_EVENTS=2,KEYBOARD_EVENTS=4;
const ENTER=0x1c,TABKEY=0x0f,UP_ARROW=0x48,DOWN_ARROW=0x50;
const RIGHT_ARROW=0x4d,LEFT_ARROW=0x4b;

void init_events();	// Initialize event manager
void setmin();			// Set minimum joystick callibrations
void setmax();			// Set maximum joystick callibrations
void setcenter();		// Set center joystick callibrations
int getevent(int);	// Get events from selected devices
