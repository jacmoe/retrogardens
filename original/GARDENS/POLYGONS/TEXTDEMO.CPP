#include  <stdio.h>
#include	<dos.h>
#include	<conio.h>
#include  <stdlib.h>
#include  "screen.h"
#include	"polytext.h"
#include  "bresnham.h"
#include  "targa.h"
#include	"bitmap.h"

void drawbox(int leftx,int lefty,int rightx,int righty,
				char *screen);

tga_struct wall;
char far bitmap[95*79];

void main()
{
	// Declare polygon structure:

	tpolytype tpoly;

	// Load wall bitmap:

	if (loadTGA("front1.tga",&wall)) exit(1);

	// Allot memory for wall bitmap:

	tpoly.textmap=new char[95*79];

	// Grab wall image in array:

	grab(0,0,95,79,320, wall.image,tpoly.textmap);

	// Create pointer to video memory:

	char far *screen=(char far *)MK_FP(0xa000,0);

	// Save previous video mode:

	int oldmode=*(int *)MK_FP(0x40,0x49);

	// Put display in mode 13h:

	setmode(0x13);

	// Set palette to ICEMAP palette:

	setpalette(wall.color_map);

	// Clear display:

	cls(screen);

	// Draw box on display:

	drawbox(60,40,95,175,screen);

	tpoly.x[0]=50;
	tpoly.y[0]=50;
	tpoly.x[1]=100;
	tpoly.y[1]=10;
	tpoly.x[2]=100;
	tpoly.y[2]=190;
	tpoly.x[3]=50;
	tpoly.y[3]=150;
	tpoly.wtext=95;
	tpoly.htext=79;
	polytext(&tpoly,60,40,95,175,screen);

	// Hold polygon on screen until keystroke

	while (!kbhit());

	// Release memory

	delete tpoly.textmap;
	delete wall.image;
	delete wall.color_map;
	if (wall.ID) delete wall.ID;

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
