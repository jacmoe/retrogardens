#include  <stdio.h>
#include	<dos.h>
#include	<conio.h>
#include  <stdlib.h>
#include  "screen.h"
#include	"polydraw.h"

void main()
{
	// Declare polygon structure:

	polytype poly;

	// Create pointer to video memory:

	char far *screen=(char far *)MK_FP(0xa000,0);

	// Save previous video mode:

	int oldmode=*(int *)MK_FP(0x40,0x49);

	// Put display in mode 13h:

	setmode(0x13);

	// Clear display:

	cls(screen);

	poly.x[0]=50;
	poly.y[0]=90;
	poly.x[1]=250;
	poly.y[1]=10;
	poly.x[2]=250;
	poly.y[2]=120;
	poly.x[3]=50;
	poly.y[3]=110;
	polydraw(&poly,0,0,0,0,1,screen);

	// Hold polygon on screen until keystroke

	while (!kbhit());

  // Restore old video mode:

  setmode(oldmode);

}
