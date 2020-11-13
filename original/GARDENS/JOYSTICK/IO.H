
const JOY_X=1, JOY_Y=2;
const JBUTTON1=0x10, JBUTTON2=0x20;
const LMBUTTON=1, RMBUTTON=2;

extern "C" {
	int readstick(int);				// Read joystick position
	int readjbutton(int);			// Read joystick button
	int	initmouse();					// Initialize mouse driver
	void rempointer();				// Remove mouse pointer from display
  void disppointer();       // Show mouse pointer on display
	int readmbutton();				// Read mouse buttons
	void relpos(int *,int *);	// Get relative mouse position
	int	initkey(char far *buffer); // Initialize keyboard handler
  int getscan();            // Get last scan code
  int remkey();             // Remove keyboard handler
}
