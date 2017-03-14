#include <stdio.h>
#include <dos.h>
#include <conio.h>
#include <stdlib.h>
#include <time.h>
#include <mem.h>
#include "screen.h"
#include "bresn.h"
#include "jmio.h"
#include "evntmngr.h"


char maze[16][16] = {
	{1, 1, 1, 1,    1, 1, 1, 1,    1, 1, 1, 1,    1, 1, 1, 1},
	{1, 0, 0, 0,    0, 0, 0, 0,    0, 0, 0, 0,    0, 0, 0, 1},
	{1, 0, 1, 0,    1, 1, 1, 0,    0, 0, 0, 0,    1, 1, 0, 1},
	{1, 0, 1, 0,    1, 0, 1, 0,    1, 1, 1, 0,    1, 1, 0, 1},

	{1, 0, 1, 0,    1, 0, 1, 0,    0, 0, 1, 0,    0, 1, 0, 1},
	{1, 0, 1, 1,    1, 0, 1, 0,    1, 0, 1, 1,    1, 1, 0, 1},
	{1, 0, 0, 0,    0, 0, 0, 0,    1, 0, 0, 0,    0, 0, 0, 1},
	{1, 0, 1, 1,    1, 0, 1, 0,    1, 1, 1, 0,    1, 1, 0, 1},

	{1, 0, 1, 0,    0, 0, 1, 0,    0, 0, 1, 0,    1, 1, 0, 1},
	{1, 0, 1, 0,    1, 0, 1, 1,    1, 1, 1, 0,    1, 1, 0, 1},
	{1, 0, 1, 0,    1, 0, 0, 0,    0, 0, 0, 0,    0, 0, 0, 1},
	{1, 0, 1, 1,    1, 0, 1, 0,    1, 0, 1, 1,    1, 1, 0, 1},

	{1, 0, 0, 0,    1, 0, 1, 0,    1, 0, 0, 0,    0, 1, 0, 1},
	{1, 0, 1, 1,    1, 0, 1, 1,    1, 1, 1, 1,    1, 1, 0, 1},
	{1, 0, 0, 0,    0, 0, 0, 0,    0, 0, 0, 0,    0, 0, 0, 1},
	{1, 1, 1, 1,    1, 1, 1, 1,    1, 1, 1, 1,    1, 1, 1, 1}
};

typedef struct xy {
	int x, y;
};

struct xy increment[4] = {{-1, 0}, {0, 1}, {1, 0}, {0, -1}};
struct xy left[4] = {{0, -1}, {-1, 0}, {0, 1}, {1, 0}};
struct xy right[4] = {{0, 1}, {1, 0}, {0, -1}, {-1, 0}};
struct xy pos = {1,3};

const int DELAY = 10000;

int lastevent = 0;

int direction = 3;
int visibility = 4;

void draw_maze();
void draw_box();

void main()
{
	struct xy newpos;

	// Input events requested by the program
	int event_mask;

	// Initialize event manager
	init_events();

	// set event mask to keyboard
	event_mask = KEYBOARD_EVENTS;

	// Clear the screen
	cls((char*)MK_FP(0xa000, 0));

	// Save previous video mode
	int oldmode = *(int*)MK_FP(0x40, 0x49);

	// Put display in mode 13h
	setgmode(0x13);

	// Create video buffer
	char far* screen_buffer = new unsigned char [64000];

	// Draw window on display
	draw_box();
	draw_maze();
	// move offscreen buffer to video ram
	//blitscreen(screen_buffer);

	// Run the loop
	while(1)
	{
		int event = getevent(event_mask);
		if(event&TABKEY) break;

		// Draw window on display
		draw_box();

		// Draw the maze
		draw_maze();

		//clrscr();
		//clrwin(0, 0, 320, 200, screen_buffer);
		//putbuffer(screen_buffer, 0, SCREEN_HEIGHT);

		// pause
		//while((clock() - last_frame) < ticks);
		//last_frame = clock();

		// Do we want to move forward?
		event = getevent(event_mask);
		if(event&UP_ARROW)
		{
			newpos.x = pos.x + increment[direction].x;
			newpos.y = pos.y + increment[direction].y;
			if(!maze[newpos.x][newpos.y])
			{
				pos.x = newpos.x;
				pos.y = newpos.y;
			}
		}
		// or do we want to go backward?
		else if(event&DOWN_ARROW)
		{
			newpos.x = pos.x - increment[direction].x;
			newpos.y = pos.y - increment[direction].y;
			if(!maze[newpos.x][newpos.y])
			{
				pos.x = newpos.x;
				pos.y = newpos.y;
			}
		}
		event = getevent(event_mask);
		// Do we want to turn left?
		if(event&LEFT_ARROW)
		{
			--direction;
			if(direction < 0)
				direction = 3;
		}
		// or do we want to turn right?
		else if(event&RIGHT_ARROW)
		{
			direction++;
			if(direction > 3)
				direction = 0;
		}
		//framect++;
		//cls(screen_buffer);
	}

	// restore old video mode
	setgmode(oldmode);
}

// Draw the maze stored in array maze[]
void draw_maze()
{
	struct xy block, lblock, rblock;
	int oldleft, oldright;

	// Draw the maze at each distance allowed by visibility
	for(int dist = 0; dist < visibility; dist++)
	{
		// Find current square of maze
		block.x = pos.x + dist * increment[direction].x;
		block.y = pos.y + dist * increment[direction].y;

		// Find square to the left of current square
		lblock.x = block.x + left[direction].x;
		lblock.y = block.y + left[direction].y;

		// Find square to the right of current square
		rblock.x = block.x + right[direction].x;
		rblock.y = block.y + right[direction].y;

		// Draw image of squares according to distance
		switch(dist)
		{
			case 0:		// Draw current square

				// Is wall open to the left?
				// If not, draw wall
				if (maze[block.x][block.y])
				{
					Line(82, 19, 135, 44, 15);
					Line(135, 44, 135, 93, 15);
					Line(135, 93, 82, 118, 15);
				}
				else // Else draw opening
				{
					Line(82, 44, 135, 44, 15);
					Line(135, 44, 135, 93, 15);
					Line(135, 93, 82, 93, 15);
				}

				// Is wall open to the right?
				// If not, draw wall
				if (maze[rblock.x][rblock.y])
				{
					Line(294, 19, 242, 44, 15);
					Line(242, 44, 242, 93, 15);
					Line(294, 118, 242, 93, 15);
				}
				else	// Else draw opening
				{
					Line(294, 44, 242, 44, 15);
					Line(242, 44, 242, 93, 15);
					Line(242, 93, 294, 93, 15);
				}
				break;

			case 1:		// Repeat for next square

				// Can we see the next square?
				// If not, draw wall
				if(maze[block.x][block.y])
				{
					Line(135, 44, 135, 93, 15);
					Line(242, 44, 242, 93, 15);
					Line(135, 44, 242, 44, 15);
					Line(135, 93, 242, 93, 15);
				}
				else	// Else draw sides of the next square
				{
					if (maze[lblock.x][lblock.y])
					{
						Line(135, 44, 162, 57, 15);
						Line(162, 57, 162, 80, 15);
						Line(162, 80, 135, 93, 15);
					}
					else
					{
						Line(135, 57, 162, 57, 15);
						Line(162, 57, 162, 80, 15);
						Line(162, 80, 135, 80, 15);
					}
					if (maze[rblock.x][rblock.y])
					{
						Line(242, 44, 215, 57, 15);
						Line(215, 57, 215, 80, 15);
						Line(215, 80, 242, 93, 15);
					}
					else
					{
						Line(242, 57, 215, 57, 15);
						Line(215, 57, 215, 80, 15);
						Line(215, 80, 242, 80, 15);
					}
				}
				break;

			case 2:		// Do it again
				if (maze[block.x][block.y])
				{
					Line(162, 57, 162, 80, 15);
					Line(215, 57, 215, 80, 15);
					Line(162, 57, 215, 57, 15);
					Line(162, 80, 215, 80, 15);
				}
				else
				{
					if (maze[lblock.x][lblock.y])
					{
						Line(162, 57, 175, 63, 15);
						Line(175, 63, 175, 74, 15);
						Line(175, 74, 162, 80, 15);
					}
					else
					{
						Line(162, 63, 175, 63, 15);
						Line(175, 63, 175, 74, 15);
						Line(175, 74, 162, 74, 15);
					}
					if (maze[rblock.x][rblock.y])
					{
						Line(215, 57, 202, 63, 15);
						Line(202, 63, 202, 74, 15);
						Line(202, 74, 215, 80, 15);
					}
					else
					{
						Line(215, 63, 202, 63, 15);
						Line(202, 63, 202, 74, 15);
						Line(202, 74, 215, 74, 15);
					}
				}
				break;

			case 3:		// And again
				if (maze[block.x][block.y])
				{
					Line(175, 63, 175, 74, 15);
					Line(202, 63, 202, 74, 15);
					Line(175, 63, 202, 63, 15);
					Line(175, 74, 202, 74, 15);
				}
				else
				{
					if (maze[lblock.x][lblock.y])
					{
						Line(175, 63, 182, 66, 15);
						Line(182, 66, 182, 70, 15);
						Line(182, 70, 175, 74, 15);
					}
					else
					{
						Line(175, 66, 182, 66, 15);
						Line(182, 66, 182, 70, 15);
						Line(182, 70, 175, 70, 15);
					}
					if (maze[rblock.x][rblock.y])
					{
						Line(202, 63, 195, 66, 15);
						Line(195, 66, 195, 70, 15);
						Line(195, 70, 202, 74, 15);
					}
					else
					{
						Line(202, 66, 195, 66, 15);
						Line(195, 66, 195, 70, 15);
						Line(195, 70, 202, 70, 15);
					}
				}
				break;
		}

		// If view is obscured by wall, stop drawing
		if(maze[block.x][block.y]) break;
	}
}

void draw_box()
{
	Line(82, 19, 294, 19, 15);
	Line(294, 19, 294, 119, 15);
	Line(294, 119, 82, 119, 15);
	Line(82, 119, 82, 19, 15);
}
