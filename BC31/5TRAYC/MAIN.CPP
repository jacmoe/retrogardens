#include <stdio.h>
#include <dos.h>
#include <conio.h>
#include <stdlib.h>
#include <mem.h>
#include <math.h>
#include "screen.h"
#include "jmkbd.h"
#include "pcx.h"

#define texWidth 64
#define texHeight 64
#define mapWidth 24
#define mapHeight 24


int worldMap[mapWidth][mapHeight]=
{
  {4,4,4,4,  4,4,4,4,  4,4,4,4,  4,4,4,4,  7,7,7,7,  7,7,7,7},
  {4,0,0,0,  0,0,0,0,  0,0,0,0,  0,0,0,0,  7,0,0,0,  0,0,0,7},
  {4,0,1,0,  0,0,0,0,  0,0,0,0,  0,0,0,0,  0,0,0,0,  0,0,0,7},
  {4,0,2,0,  0,0,0,0,  0,0,0,0,  0,0,0,0,  0,0,0,0,  0,0,0,7},

  {4,0,3,0,  0,0,0,0,  0,0,0,0,  0,0,0,0,  7,0,0,0,  0,0,0,7},
  {4,0,4,0,  0,0,0,5,  5,5,5,5,  5,5,5,5,  7,7,0,7,  7,7,7,7},
  {4,0,5,0,  0,0,0,5,  0,5,0,5,  0,5,0,5,  7,0,0,0,  7,7,7,1},
  {4,0,6,0,  0,0,0,5,  0,0,0,0,  0,0,0,5,  7,0,0,0,  0,0,0,8},

  {4,0,7,0,  0,0,0,0,  0,0,0,0,  0,0,0,0,  0,0,0,0,  7,7,7,1},
  {4,0,8,0,  0,0,0,5,  0,0,0,0,  0,0,0,5,  7,0,0,0,  0,0,0,8},
  {4,0,0,0,  0,0,0,5,  0,0,0,0,  0,0,0,5,  7,0,0,0,  7,7,7,1},
  {4,0,0,0,  0,0,0,5,  5,5,5,0,  5,5,5,5,  7,7,7,7,  7,7,7,1},

  {6,6,6,6,  6,6,6,6,  6,6,6,0,  6,6,6,6,  6,6,6,6,  6,6,6,6},
  {8,0,0,0,  0,0,0,0,  0,0,0,0,  0,0,0,0,  0,0,0,0,  0,0,0,4},
  {6,6,6,6,  6,6,0,6,  6,6,6,0,  6,6,6,6,  6,6,6,6,  6,6,6,6},
  {4,4,4,4,  4,4,0,4,  4,4,6,0,  6,2,2,2,  2,2,2,2,  3,3,3,3},

  {4,0,0,0,  0,0,0,0,  0,4,6,0,  6,2,0,0,  0,0,0,2,  0,0,0,2},
  {4,0,0,0,  0,0,0,0,  0,0,0,0,  6,2,0,0,  5,0,0,2,  0,0,0,2},
  {4,0,0,0,  0,0,0,0,  0,4,6,0,  6,2,0,0,  0,0,0,2,  2,0,2,2},
  {4,0,6,0,  6,0,0,0,  0,4,6,0,  0,0,0,0,  5,0,0,0,  0,0,0,2},

  {4,0,0,5,  0,0,0,0,  0,4,6,0,  6,2,0,0,  0,0,0,2,  2,0,2,2},
  {4,0,6,0,  6,0,0,0,  0,4,6,0,  6,2,0,0,  5,0,0,2,  0,0,0,2},
  {4,0,0,0,  0,0,0,0,  0,4,6,0,  6,2,0,0,  0,0,0,2,  0,0,0,2},
  {4,4,4,4,  4,4,4,4,  4,4,1,1,  1,2,2,2,  2,2,2,3,  3,3,3,3}
};

word* game_clock = (word*)0x0000046C; // points to 18.2hz system clock

PcxImage bitmaps[8];

void main()
{
	// load the bitmaps
	PcxLoadImage("TEX\\STONE1.PCX", &bitmaps[0]);
	PcxLoadImage("TEX\\STONE3.PCX", &bitmaps[1]);
	PcxLoadImage("TEX\\BLUE.PCX", &bitmaps[2]);
	PcxLoadImage("TEX\\PURPLE.PCX", &bitmaps[3]);
	PcxLoadImage("TEX\\MOSSY.PCX", &bitmaps[4]);
	PcxLoadImage("TEX\\EAGLE.PCX", &bitmaps[5]);
	PcxLoadImage("TEX\\BRICK.PCX", &bitmaps[6]);
	PcxLoadImage("TEX\\WOOD.PCX", &bitmaps[7]);
	///*
	for(int p = 0; p < 8; p++)
	{
		TransposeBitmap(bitmaps[p].bitmap, bitmaps[p].width, bitmaps[p].height);
	}//*/

	// Put display in mode 13h
	set_mode(VGA_256_COLOR_MODE);

	set_palette(bitmaps[0].pal);

	// Double buffer
	byte* double_buffer;

	if((double_buffer = (byte*) malloc(SCREEN_SIZE)) == NULL)
	{
		printf("Not enough memory for double buffer.\n");
		exit(1);
	}

	double time = 0; // time of current frame
	double oldTime = 0; // time of previous frame

	double rotateFactor = 3.0;
	double moveFactor = 10.0;

	Keyboard kb;

	double posX = 22.0, posY = 11.5; // x and y starting positions
	double dirX = -1.0, dirY = 0.0; // initial direction vector
	double planeX = 0.0, planeY = 0.66; // 2d raycaster version of camera plane

	// Wait for the user to hit escape key
	while(!kb.wasPressed(esckey))
	{
		// clear double buffer
		memset(double_buffer, 8, SCREEN_SIZE);

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

			// texturing calculations
			int texNum = worldMap[mapX][mapY] - 1; // 1 subtracted so that texture 0 can be used

			// calculate value of wallX
			double wallX; // where the wall was hit
			if (side == 0)
			{
				wallX = rayPosY + perpWallDist * rayDirY;
			}
			else
			{
				wallX = rayPosX + perpWallDist * rayDirX;
			}
			wallX -= floor((wallX));

			// x coordinate on the texture
			int texX = int(wallX * double(texWidth));
			if(side == 0 && rayDirX > 0) texX = texWidth - texX - 1;
			if(side == 1 && rayDirY < 0) texX = texWidth - texX - 1;

			for(int y = drawStart; y < drawEnd; y++)
			{
				int d = y * 256 - SCREEN_HEIGHT * 128 + lineHeight * 128; // 256 and 128 factors to avoid floats
				int texY = ((d * texHeight) / lineHeight) / 256;
				byte color = bitmaps[texNum].bitmap[texHeight * texY + texX];
				// make color darker for y-sides: R, G and B byte each divided through with a shift and an and
				if(side == 1) color = (color >> 1) & 8355711;

				double_buffer[(y << 8) + (y << 6) + x] = color;
				//double_buffer[x + (y << 8) + (y << 6)] = color;
				//plot_pixel_buf(x, y, color, double_buffer);
			}
		}
		oldTime = time;
		time = *game_clock;
		double frameTime = (time - oldTime) / 18.2;

		// show the buffer
		show_buffer(double_buffer);

		double moveSpeed = frameTime * moveFactor;
		double rotSpeed = frameTime * rotateFactor;

		// move forwards if no wall in front of you
		if(kb.isKeyDown(uparrow))
		{
			if(worldMap[int(posX + dirX * moveSpeed)][int(posY)] == 0)
			{
				posX += dirX * moveSpeed;
			}
			if(worldMap[int(posX)][int(posY + dirY * moveSpeed)] == 0)
			{
				posY += dirY * moveSpeed;
			}
		}
		// move backwards if no wall in front of you
		if(kb.isKeyDown(dnarrow))
		{
			if(worldMap[int(posX - dirX * moveSpeed)][int(posY)] == 0)
			{
				posX -= dirX * moveSpeed;
			}
			if(worldMap[int(posX)][int(posY - dirY * moveSpeed)] == 0)
			{
				posY -= dirY * moveSpeed;
			}
		}
		// rotate to the right
		if(kb.isKeyDown(rtarrow))
		{
			// both camera direction and camera plane must be rotated
			double oldDirX = dirX;
			dirX = dirX * cos(-rotSpeed) - dirY * sin(-rotSpeed);
			dirY = oldDirX * sin(-rotSpeed) + dirY * cos(-rotSpeed);
			double oldPlaneX = planeX;
			planeX = planeX * cos(-rotSpeed) - planeY * sin(-rotSpeed);
			planeY = oldPlaneX * sin(-rotSpeed) + planeY * cos(-rotSpeed);
		}
		// rotate to the left
		if(kb.isKeyDown(lfarrow))
		{
			// both camera direction and camera plane must be rotated
			double oldDirX = dirX;
			dirX = dirX * cos(rotSpeed) - dirY * sin(rotSpeed);
			dirY = oldDirX * sin(rotSpeed) + dirY * cos(rotSpeed);
			double oldPlaneX = planeX;
			planeX = planeX * cos(rotSpeed) - planeY * sin(rotSpeed);
			planeY = oldPlaneX * sin(rotSpeed) + planeY * cos(rotSpeed);
		}
	} // while not escape key pressed

	// free resources
	/*
	for(int res = 0; res < 8; res++)
	{
		free(bitmaps[res].data);
	}
	*/
	free(double_buffer);

	// enter text mode
	set_mode(TEXT_MODE);
}
