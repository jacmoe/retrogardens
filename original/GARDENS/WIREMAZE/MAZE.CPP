
// MAZE.CPP Version 1.0
// Draw a single view inside a 3D wireframe maze
//
// Written by Christopher Lampton for
// for GARDENS OF IMAGINATION (Waite Group Press)

#include  <stdio.h>
#include  <dos.h>
#include  <conio.h>
#include  <stdlib.h>
#include  "screen.h"
#include  "bresn.h"

char maze[16][16]={
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{1,0,1,0,1,1,1,0,1,0,1,0,1,1,0,1},
	{1,0,1,0,1,0,0,0,1,1,1,0,1,1,0,1},
	{1,0,1,0,0,0,1,0,0,0,1,0,0,1,0,1},
	{1,0,1,1,1,0,1,0,1,0,1,1,1,1,0,1},
	{1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,1},
	{1,0,1,1,1,0,1,0,1,1,1,0,1,1,0,1},
	{1,0,1,0,0,0,1,1,0,0,1,0,1,1,0,1},
	{1,0,1,0,1,0,1,1,1,1,1,0,1,1,0,1},
	{1,0,1,0,1,0,0,0,0,0,0,0,0,0,0,1},
	{1,0,1,1,1,0,1,0,1,0,1,1,1,1,0,1},
	{1,0,0,0,1,0,1,0,1,0,0,0,0,1,0,1},
	{1,0,1,1,1,0,1,1,1,1,1,1,1,1,0,1},
	{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};

typedef struct xy {
	int x,y;
};

struct xy increment[4]={{-1,0},{0,1},{1,0},{0,-1}};
struct xy left[4]={{0,-1},{-1,0},{0,1},{1,0}};
struct xy right[4]={{0,1},{1,0},{0,-1},{-1,0}};
struct xy pos={5,5};

int direction=0;
int visibility=4;

void drawmaze(char *);
void drawbox(char *);

void main(void)
{

	// Create pointer to video memory:

	char far *screen=(char far *)MK_FP(0xa000,0);

	// Save previous video mode:

	int oldmode=*(int *)MK_FP(0x40,0x49);

	// Put display in mode 13h:

	setmode(0x13);

	// Clear display:

	cls(screen);

	// Draw window on display:

	drawbox(screen);

	// Draw the maze:

	drawmaze(screen);

	// Wait for user to hit a key:

	while (!kbhit());

	// Restore old video mode:

	setmode(oldmode);
}

void drawbox(char *screen)
{
	linedraw(82,19,294,19,15,screen);
	linedraw(294,19,294,119,15,screen);
	linedraw(294,119,82,119,15,screen);
	linedraw(82,119,82,19,15,screen);
}

void drawmaze(char *screen)

// Draw the maze stored in array maze[] into video
// memory at address screen.

{
	struct xy block,lblock,rblock;
	int oldleft,oldright;

	// Draw the maze at each distance allowed by visibility

	for(int dist=0; dist<visibility; dist++) {

		// Find current square of maze:

		block.x=pos.x+dist*increment[direction].x;
		block.y=pos.y+dist*increment[direction].y;

		// Find square to the left of current square:

		lblock.x=block.x+left[direction].x;
		lblock.y=block.y+left[direction].y;

		// Find square to the right of current square:

		rblock.x=block.x+right[direction].x;
		rblock.y=block.y+right[direction].y;

		// Draw image of squares according to distance:

		switch(dist) {
			case 0:  // Draw current square

				// Is wall open to the left?
				// If not, draw wall

				if (maze[lblock.x][lblock.y]) {
					linedraw(82,19,135,44,15,screen);
					linedraw(135,44,135,93,15,screen);
					linedraw(135,93,82,118,15,screen);
				}
				else { // Else draw opening
					linedraw(82,44,135,44,15,screen);
					linedraw(135,44,135,93,15,screen);
					linedraw(135,93,82,93,15,screen);
				}

				// Is wall open to the right?
				// If not, draw wall

				if (maze[rblock.x][rblock.y]) {
					linedraw(294,19,242,44,15,screen);
					linedraw(242,44,242,93,15,screen);
					linedraw(294,118,242,93,15,screen);
				}
				else { // Else draw opening
					linedraw(294,44,242,44,15,screen);
					linedraw(242,44,242,93,15,screen);
					linedraw(242,93,294,93,15,screen);
				}
				break;
			case 1: // Repeat for next square

				// Can we see through the next square?
				// If not, draw wall

				if (maze[block.x][block.y]) {
					linedraw(135,44,135,93,15,screen);
					linedraw(242,44,242,93,15,screen);
					linedraw(135,44,242,44,15,screen);
					linedraw(135,93,242,93,15,screen);
				}
				else { // Else draw sides of next square
					if (maze[lblock.x][lblock.y]) {
						linedraw(135,44,162,57,15,screen);
						linedraw(162,57,162,80,15,screen);
						linedraw(162,80,135,93,15,screen);
					}
					else {
						linedraw(135,57,162,57,15,screen);
						linedraw(162,57,162,80,15,screen);
						linedraw(162,80,135,80,15,screen);
					}
					if (maze[rblock.x][rblock.y]) {
						linedraw(242,44,215,57,15,screen);
						linedraw(215,57,215,80,15,screen);
						linedraw(215,80,242,93,15,screen);
					}
					else {
						linedraw(242,57,215,57,15,screen);
						linedraw(215,57,215,80,15,screen);
						linedraw(215,80,242,80,15,screen);
					}
				}
				break;
			case 2: // Do it again
				if (maze[block.x][block.y]) {
					linedraw(162,57,162,80,15,screen);
					linedraw(215,57,215,80,15,screen);
					linedraw(162,57,215,57,15,screen);
					linedraw(162,80,215,80,15,screen);
				}
				else {
					if (maze[lblock.x][lblock.y]) {
						linedraw(162,57,175,63,15,screen);
						linedraw(175,63,175,74,15,screen);
						linedraw(175,74,162,80,15,screen);
					}
					else {
						linedraw(162,63,175,63,15,screen);
						linedraw(175,63,175,74,15,screen);
						linedraw(175,74,162,74,15,screen);
					}
					if (maze[rblock.x][rblock.y]) {
						linedraw(215,57,202,63,15,screen);
						linedraw(202,63,202,74,15,screen);
						linedraw(202,74,215,80,15,screen);
					}
					else {
						linedraw(215,63,202,63,15,screen);
						linedraw(202,63,202,74,15,screen);
						linedraw(202,74,215,74,15,screen);
					}
				}
				break;
			case 3: // And again
				if (maze[block.x][block.y]) {
					linedraw(175,63,175,74,15,screen);
					linedraw(202,63,202,74,15,screen);
					linedraw(175,63,202,63,15,screen);
					linedraw(175,74,202,74,15,screen);
				}
				else {
					if (maze[lblock.x][lblock.y]) {
						linedraw(175,63,182,66,15,screen);
						linedraw(182,66,182,70,15,screen);
						linedraw(182,70,175,74,15,screen);
					}
					else {
						linedraw(175,66,182,66,15,screen);
						linedraw(182,66,182,70,15,screen);
						linedraw(182,70,175,70,15,screen);
					}
					if (maze[rblock.x][rblock.y]) {
						linedraw(202,63,195,66,15,screen);
						linedraw(195,66,195,70,15,screen);
						linedraw(195,70,202,74,15,screen);
					}
					else {
						linedraw(202,66,195,66,15,screen);
						linedraw(195,66,195,70,15,screen);
						linedraw(195,70,202,70,15,screen);
					}
				}
				break;
		}

		// If view is obscured by wall, stop drawing:

		if (maze[block.x][block.y]) break;
	}
}
