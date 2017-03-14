#include <stdio.h>
#include <dos.h>
#include <conio.h>
#include <stdlib.h>
#include "screen.h"

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

int direction = 3;
int visibility = 4;

void draw_maze();
void draw_box();

void main()
{
	// Put display in mode 13h
	set_mode(VGA_256_COLOR_MODE);

	// Draw window on display
	draw_box();

	// Draw the maze
	draw_maze();

	// Wait for the user to hit a key
	while(!kbhit());

	// restore old video mode
	set_mode(TEXT_MODE);
}

// Draw the maze stored in array maze[] into video
// memory at address screen.
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
					line(82, 19, 135, 44, 15);
					line(135, 44, 135, 93, 15);
					line(135, 93, 82, 118, 15);
				}
				else // Else draw opening
				{
					line(82, 44, 135, 44, 15);
					line(135, 44, 135, 93, 15);
					line(135, 93, 82, 93, 15);
				}

				// Is wall open to the right?
				// If not, draw wall
				if (maze[rblock.x][rblock.y])
				{
					line(294, 19, 242, 44, 15);
					line(242, 44, 242, 93, 15);
					line(294, 118, 242, 93, 15);
				}
				else	// Else draw opening
				{
					line(294, 44, 242, 44, 15);
					line(242, 44, 242, 93, 15);
					line(242, 93, 294, 93, 15);
				}
				break;

			case 1:		// Repeat for next square

				// Can we see the next square?
				// If not, draw wall
				if(maze[block.x][block.y])
				{
					line(135, 44, 135, 93, 15);
					line(242, 44, 242, 93, 15);
					line(135, 44, 242, 44, 15);
					line(135, 93, 242, 93, 15);
				}
				else	// Else draw sides of the next square
				{
					if (maze[lblock.x][lblock.y])
					{
						line(135, 44, 162, 57, 15);
						line(162, 57, 162, 80, 15);
						line(162, 80, 135, 93, 15);
					}
					else
					{
						line(135, 57, 162, 57, 15);
						line(162, 57, 162, 80, 15);
						line(162, 80, 135, 80, 15);
					}
					if (maze[rblock.x][rblock.y])
					{
						line(242, 44, 215, 57, 15);
						line(215, 57, 215, 80, 15);
						line(215, 80, 242, 93, 15);
					}
					else
					{
						line(242, 57, 215, 57, 15);
						line(215, 57, 215, 80, 15);
						line(215, 80, 242, 80, 15);
					}
				}
				break;

			case 2:		// Do it again
				if (maze[block.x][block.y])
				{
					line(162, 57, 162, 80, 15);
					line(215, 57, 215, 80, 15);
					line(162, 57, 215, 57, 15);
					line(162, 80, 215, 80, 15);
				}
				else
				{
					if (maze[lblock.x][lblock.y])
					{
						line(162, 57, 175, 63, 15);
						line(175, 63, 175, 74, 15);
						line(175, 74, 162, 80, 15);
					}
					else
					{
						line(162, 63, 175, 63, 15);
						line(175, 63, 175, 74, 15);
						line(175, 74, 162, 74, 15);
					}
					if (maze[rblock.x][rblock.y])
					{
						line(215, 57, 202, 63, 15);
						line(202, 63, 202, 74, 15);
						line(202, 74, 215, 80, 15);
					}
					else
					{
						line(215, 63, 202, 63, 15);
						line(202, 63, 202, 74, 15);
						line(202, 74, 215, 74, 15);
					}
				}
				break;

			case 3:		// And again
				if (maze[block.x][block.y])
				{
					line(175, 63, 175, 74, 15);
					line(202, 63, 202, 74, 15);
					line(175, 63, 202, 63, 15);
					line(175, 74, 202, 74, 15);
				}
				else
				{
					if (maze[lblock.x][lblock.y])
					{
						line(175, 63, 182, 66, 15);
						line(182, 66, 182, 70, 15);
						line(182, 70, 175, 74, 15);
					}
					else
					{
						line(175, 66, 182, 66, 15);
						line(182, 66, 182, 70, 15);
						line(182, 70, 175, 70, 15);
					}
					if (maze[rblock.x][rblock.y])
					{
						line(202, 63, 195, 66, 15);
						line(195, 66, 195, 70, 15);
						line(195, 70, 202, 74, 15);
					}
					else
					{
						line(202, 66, 195, 66, 15);
						line(195, 66, 195, 70, 15);
						line(195, 70, 202, 70, 15);
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
	//left, top, right, bottom
	rect(82, 19, 294, 119, 15);
}
