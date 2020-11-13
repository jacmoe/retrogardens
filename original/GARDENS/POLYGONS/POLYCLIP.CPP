//
// POLYCLIP.CPP
//
// Clips a filled polygon against a rectangle
//
// Written by Christopher Lampton for
// GARDENS OF IMAGINATION (Waite Group Press, 1994)
//

#include  <stdio.h>
#include	<dos.h>
#include	<conio.h>
#include  <stdlib.h>
#include  "screen.h"
#include	"polydraw.h"
#include  "bresnham.h"

void drawbox(int leftx,int lefty,int rightx,int righty,
				char *screen);

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

	// Draw box on display:

	drawbox(70,50,200,130,screen);

	poly.x[0]=50;
	poly.y[0]=90;
	poly.x[1]=250;
	poly.y[1]=10;
	poly.x[2]=250;
	poly.y[2]=120;
	poly.x[3]=50;
	poly.y[3]=110;
	polydraw(&poly,71,51,199,129,1,screen);
//	polydraw(&poly,0,0,319,199,1,screen);

	// Hold polygon on screen until keystroke

	while (!kbhit());

	// Restore old video mode:

	setmode(oldmode);

}

void drawbox(int leftx,int lefty,int rightx,int righty,
			 char *screen)
{
	linedraw(leftx,lefty,rightx,lefty,15,screen);
	linedraw(rightx,lefty,rightx,righty,15,screen);
	linedraw(rightx,righty,leftx,righty,15,screen);
	linedraw(leftx,righty,leftx,lefty,15,screen);
}
