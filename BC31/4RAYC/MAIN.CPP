#include <stdio.h>
#include <dos.h>
#include <conio.h>
#include <stdlib.h>
#include <mem.h>
#include <math.h>
#include "screen.h"
#include "jmkbd.h"

#define mapWidth 24
#define mapHeight 24

int worldMap[mapWidth][mapHeight]=
{
  {1,1,1,1,  1,1,1,1,  1,1,1,1,  1,1,1,1,  1,1,1,1,  1,1,1,1},
  {1,0,0,0,  0,0,0,0,  0,0,0,0,  0,0,0,0,  0,0,0,0,  0,0,0,1},
  {1,0,0,0,  0,0,0,0,  0,0,0,0,  0,0,0,0,  0,0,0,0,  0,0,0,1},
  {1,0,0,0,  0,0,0,0,  0,0,0,0,  0,0,0,0,  0,0,0,0,  0,0,0,1},

  {1,0,0,0,  0,0,2,2,  2,2,2,0,  0,0,0,3,  0,3,0,3,  0,0,0,1},
  {1,0,0,0,  0,0,2,0,  0,0,2,0,  0,0,0,0,  0,0,0,0,  0,0,0,1},
  {1,0,0,0,  0,0,2,0,  0,0,2,0,  0,0,0,3,  0,0,0,3,  0,0,0,1},
  {1,0,0,0,  0,0,2,0,  0,0,2,0,  0,0,0,0,  0,0,0,0,  0,0,0,1},

  {1,0,0,0,  0,0,2,2,  0,2,2,0,  0,0,0,3,  0,3,0,3,  0,0,0,1},
  {1,0,0,0,  0,0,0,0,  0,0,0,0,  0,0,0,0,  0,0,0,0,  0,0,0,1},
  {1,0,0,0,  0,0,0,0,  0,0,0,0,  0,0,0,0,  0,0,0,0,  0,0,0,1},
  {1,0,0,0,  0,0,0,0,  0,0,0,0,  0,0,0,0,  0,0,0,0,  0,0,0,1},

  {1,0,0,0,  0,0,0,0,  0,0,0,0,  0,0,0,0,  0,0,0,0,  0,0,0,1},
  {1,0,0,0,  0,0,0,0,  0,0,0,0,  0,0,0,0,  0,0,0,0,  0,0,0,1},
  {1,0,0,0,  0,0,0,0,  0,0,0,0,  0,0,0,0,  0,0,0,0,  0,0,0,1},
  {1,0,0,0,  0,0,0,0,  0,0,0,0,  0,0,0,0,  0,0,0,0,  0,0,0,1},

  {1,4,4,4,  4,4,4,4,  4,0,0,0,  0,0,0,0,  0,0,0,0,  0,0,0,1},
  {1,4,0,4,  0,0,0,0,  4,0,0,0,  0,0,0,0,  0,0,0,0,  0,0,0,1},
  {1,4,0,0,  0,0,5,0,  4,0,0,0,  0,0,0,0,  0,0,0,0,  0,0,0,1},
  {1,4,0,4,  0,0,0,0,  4,0,0,0,  0,0,0,0,  0,0,0,0,  0,0,0,1},

  {1,4,0,4,  4,4,4,4,  4,0,0,0,  0,0,0,0,  0,0,0,0,  0,0,0,1},
  {1,4,0,0,  0,0,0,0,  0,0,0,0,  0,0,0,0,  0,0,0,0,  0,0,0,1},
  {1,4,4,4,  4,4,4,4,  4,0,0,0,  0,0,0,0,  0,0,0,0,  0,0,0,1},
  {1,1,1,1,  1,1,1,1,  1,1,1,1,  1,1,1,1,  1,1,1,1,  1,1,1,1}
};

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

	Keyboard kb;

	double posX = 22, posY = 12;
	double dirX = -1, dirY = 0;
	double planeX = 0, planeY = 0.66;

	// Wait for the user to hit escape key
	while(!kb.wasPressed(esckey))
	{
		// clear double buffer
		memset(double_buffer, 0, SCREEN_SIZE);

		for(int x = 0; x < SCREEN_WIDTH; x++)
		{
			// calculate ray position and direction
			double cameraX = 2 * x / double(SCREEN_WIDTH) - 1;
			double rayPosX = posX;
			double rayPosY = posY;
			double rayDirX = dirX + planeX * cameraX;
			double rayDirY = dirY + planeY * cameraX;

			// which box of the map we're in
			int mapX = int(rayPosX);
			int mapY = int(rayPosY);

			// length of ray from current position to next x or y-side
			double sideDistX;
			double sideDistY;

			// length of ray from one x or y-side to next x or y-side
			double deltaDistX = sqrt(1 + (rayDirY * rayDirY) / (rayDirX * rayDirX));
			double deltaDistY = sqrt(1 + (rayDirX * rayDirX) / (rayDirY * rayDirY));
			double perpWallDist;

			// what direction to step in x or y-direction (either +1 or -1)
			int stepX;
			int stepY;

			int hit = 0; // was there a wall hit?
			int side; // was a NS or a EW wall hit?

			if(rayDirX < 0)
			{
				stepX = -1;
				sideDistX = (rayPosX - mapX) * deltaDistX;
			}
			else
			{
				stepX = 1;
				sideDistX = (mapX + 1.0 - rayPosX) * deltaDistX;
			}
			if(rayDirY < 0)
			{
				stepY = -1;
				sideDistY = (rayPosY - mapY) * deltaDistY;
			}
			else
			{
				stepY = 1;
				sideDistY = (mapY + 1.0 - rayPosY) * deltaDistY;
			}
			// perform DDA
			while(hit == 0)
			{
				// jump to the next map square, OR in x-direction, OR in y-direction
				if(sideDistX < sideDistY)
				{
					sideDistX += deltaDistX;
					mapX += stepX;
					side = 0;
				}
				else
				{
					sideDistY += deltaDistY;
					mapY += stepY;
					side = 1;
				}
				// check if ray has hit a wall
				if(worldMap[mapX][mapY] > 0) hit = 1;
			}
			// calculate distance projected on camera direction
			// oblique distance will give fisheye effect!
			if(side == 0)
			{
            	perpWallDist = (mapX - rayPosX + (1 - stepX) / 2) / rayDirX;
			}
			else
			{
				perpWallDist = (mapY - rayPosY + (1 - stepY) / 2) / rayDirY;
			}

			// calculate height of line to draw on screen
			int lineHeight = (int)(SCREEN_HEIGHT / perpWallDist);

			// calculate lowest and highest pixel to fill in current stripe
			int drawStart = -lineHeight / 2 + SCREEN_HEIGHT / 2;
			if(drawStart < 0) drawStart = 0;
			int drawEnd = lineHeight / 2 + SCREEN_HEIGHT / 2;
			if(drawEnd >= SCREEN_HEIGHT) drawEnd = SCREEN_HEIGHT - 1;

			// TODO: choose color

			// TODO: x and y sides different brightness

			// draw the pixels of the stripe as a vertical line
			vert_line_buf(x, drawStart, drawEnd, worldMap[mapX][mapY], double_buffer);
		}
		// show the buffer
		show_buffer(double_buffer);

	} // while not escape key pressed

	// free resources
	free(double_buffer);

	// enter text mode
	set_mode(TEXT_MODE);
}
















