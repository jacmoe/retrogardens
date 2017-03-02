// IO.H
// include file containing prototype for assembler implementation of
// input functions in IO.ASM

#ifndef __IOASM__
#define __IOASM__

const JOY_X = 1, JOY_Y = 2;
const JBUTTON1 = 0x10, JBUTTON2 = 0x20, MBUTTON1 = 1, MBUTTON2 = 2;


extern "C" {
    // Read joystick position
    int readstick(int);
    // Read joystick button
    int readjbutton(int);
    // Initialize mouse driver
    int initmouse(void);
    // Remove mouse pointer from display
    void rempointer(void);
    // Read mouse buttons
    int readmbutton(void);
    // Get relative mouse position
    void relpos(int *,int *);
    // Get keyboard scan code
    int scankey(void);
}

#endif
