#include <stdio.h>
#include <dos.h>
#include <conio.h>
#include <stdlib.h>
#include <mem.h>
#include "screen.h"
#include "jmkbd.h"


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

void draw_maze(byte* buffer);
void draw_box(byte* buffer);

void main()
{
	// Put display in mode 13h
	set_mode(VGA_256_COLOR_MODE);

	// Double buffer
	byte* double_buffer;

	if((double_buffer = (byte*) malloc(SCREEN_SIZE)) == NULL)
	{
		printf("Not enough memory for double buffer.\n");
		exit(1);
	}

	struct xy newpos;

	Keyboard kb;

	// Wait for the user to hit escape key
	while(!kb.wasPressed(esckey))
	{
		// clear double buffer
		memset(double_buffer, 0, SCREEN_SIZE);

		// Draw window on display
		draw_box(double_buffer);

		// Draw the maze
		draw_maze(double_buffer);

		// show the buffer
        show_buffer(double_buffer);

		// Do we want to move forward?
		if(kb.isKeyDown(uparrow))
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
		else if(kb.isKeyDown(dnarrow))
		{
			newpos.x = pos.x - increment[direction].x;
			newpos.y = pos.y - increment[direction].y;
			if(!maze[newpos.x][newpos.y])
			{
				pos.x = newpos.x;
				pos.y = newpos.y;
			}
		}
		// Do we want to turn left?
		if(kb.isKeyDown(rtarrow))
		{
			--direction;
			if(direction < 0)
				direction = 3;
		}
		// or do we want to turn right?
		else if(kb.isKeyDown(lfarrow))
		{
			direction++;
			if(direction > 3)
				direction = 0;
		}
	} // while not escape key pressed

	// free resources
	free(double_buffer);

	// enter text mode
	set_mode(TEXT_MODE);
}

// Draw the maze stored in array maze[] into video
// memory at address screen.
void draw_maze(byte* buffer)
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
					line_buf(82, 19, 135, 44, 15, buffer);
					line_buf(135, 44, 135, 93, 15, buffer);
					line_buf(135, 93, 82, 118, 15, buffer);
				}
				else // Else draw opening
				{
					line_buf(82, 44, 135, 44, 15, buffer);
					line_buf(135, 44, 135, 93, 15, buffer);
					line_buf(135, 93, 82, 93, 15, buffer);
				}

				// Is wall open to the right?
				// If not, draw wall
				if (maze[rblock.x][rblock.y])
				{
					line_buf(294, 19, 242, 44, 15, buffer);
					line_buf(242, 44, 242, 93, 15, buffer);
					line_buf(294, 118, 242, 93, 15, buffer);
				}
				else	// Else draw opening
				{
					line_buf(294, 44, 242, 44, 15, buffer);
					line_buf(242, 44, 242, 93, 15, buffer);
					line_buf(242, 93, 294, 93, 15, buffer);
				}
				break;

			case 1:		// Repeat for next square

				// Can we see the next square?
				// If not, draw wall
				if(maze[block.x][block.y])
				{
					line_buf(135, 44, 135, 93, 15, buffer);
					line_buf(242, 44, 242, 93, 15, buffer);
					line_buf(135, 44, 242, 44, 15, buffer);
					line_buf(135, 93, 242, 93, 15, buffer);
				}
				else	// Else draw sides of the next square
				{
					if (maze[lblock.x][lblock.y])
					{
						line_buf(135, 44, 162, 57, 15, buffer);
						line_buf(162, 57, 162, 80, 15, buffer);
						line_buf(162, 80, 135, 93, 15, buffer);
					}
					else
					{
						line_buf(135, 57, 162, 57, 15, buffer);
						line_buf(162, 57, 162, 80, 15, buffer);
						line_buf(162, 80, 135, 80, 15, buffer);
					}
					if (maze[rblock.x][rblock.y])
					{
						line_buf(242, 44, 215, 57, 15, buffer);
						line_buf(215, 57, 215, 80, 15, buffer);
						line_buf(215, 80, 242, 93, 15, buffer);
					}
					else
					{
						line_buf(242, 57, 215, 57, 15, buffer);
						line_buf(215, 57, 215, 80, 15, buffer);
						line_buf(215, 80, 242, 80, 15, buffer);
					}
				}
				break;

			case 2:		// Do it again
				if (maze[block.x][block.y])
				{
					line_buf(162, 57, 162, 80, 15, buffer);
					line_buf(215, 57, 215, 80, 15, buffer);
					line_buf(162, 57, 215, 57, 15, buffer);
					line_buf(162, 80, 215, 80, 15, buffer);
				}
				else
				{
					if (maze[lblock.x][lblock.y])
					{
						line_buf(162, 57, 175, 63, 15, buffer);
						line_buf(175, 63, 175, 74, 15, buffer);
						line_buf(175, 74, 162, 80, 15, buffer);
					}
					else
					{
						line_buf(162, 63, 175, 63, 15, buffer);
						line_buf(175, 63, 175, 74, 15, buffer);
						line_buf(175, 74, 162, 74, 15, buffer);
					}
					if (maze[rblock.x][rblock.y])
					{
						line_buf(215, 57, 202, 63, 15, buffer);
						line_buf(202, 63, 202, 74, 15, buffer);
						line_buf(202, 74, 215, 80, 15, buffer);
					}
					else
					{
						line_buf(215, 63, 202, 63, 15, buffer);
						line_buf(202, 63, 202, 74, 15, buffer);
						line_buf(202, 74, 215, 74, 15, buffer);
					}
				}
				break;

			case 3:		// And again
				if (maze[block.x][block.y])
				{
					line_buf(175, 63, 175, 74, 15, buffer);
					line_buf(202, 63, 202, 74, 15, buffer);
					line_buf(175, 63, 202, 63, 15, buffer);
					line_buf(175, 74, 202, 74, 15, buffer);
				}
				else
				{
					if (maze[lblock.x][lblock.y])
					{
						line_buf(175, 63, 182, 66, 15, buffer);
						line_buf(182, 66, 182, 70, 15, buffer);
						line_buf(182, 70, 175, 74, 15, buffer);
					}
					else
					{
						line_buf(175, 66, 182, 66, 15, buffer);
						line_buf(182, 66, 182, 70, 15, buffer);
						line_buf(182, 70, 175, 70, 15, buffer);
					}
					if (maze[rblock.x][rblock.y])
					{
						line_buf(202, 63, 195, 66, 15, buffer);
						line_buf(195, 66, 195, 70, 15, buffer);
						line_buf(195, 70, 202, 74, 15, buffer);
					}
					else
					{
						line_buf(202, 66, 195, 66, 15, buffer);
						line_buf(195, 66, 195, 70, 15, buffer);
						line_buf(195, 70, 202, 70, 15, buffer);
					}
				}
				break;
		}

		// If view is obscured by wall, stop drawing
		if(maze[block.x][block.y]) break;
	}
}

void draw_box(byte* buffer)
{
	//left, top, right, bottom
	rect_buf(82, 19, 294, 119, 15, buffer);
}
