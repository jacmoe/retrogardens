
const JOY_X=1, JOY_Y=2;
const JBUTTON1=0x10, JBUTTON2=0x20, MBUTTON1=1, MBUTTON2=2;

extern "C" {
	int readstick(int);				// Read joystick position
	int readjbutton(int);			// Read joystick button
	int	initmouse();					// Initialize mouse driver
	void rempointer();				// Remove mouse pointer from display
	int readmbutton();				// Read mouse buttons
	void relpos(int *,int *);	// Get relative mouse position
	int	scankey();						// Get keyboard scan code
}
