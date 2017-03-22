/*
	Utility for generating lightsourcing tables.
	Written by Christoper Lampton
	for Gardens of Imagination (Waite Group Press)

	// painstakingly handwritten from the book by Jacob 'jacmoe' Moen
*/

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <values.h>
#include <conio.h>
#include <fcntl.h>
#include "pcx.h"

const MAXLIGHT = 32;
const PALETTE_SIZE = 256;

PcxImage pcx;
int red[PALETTE_SIZE], green[PALETTE_SIZE], blue[PALETTE_SIZE];
unsigned char litesource[MAXLIGHT + 1][PALETTE_SIZE];
float redscore, greenscore, bluescore;
FILE* file_handle;

void main(int argc, char* argv[])
{
	// set default target to black:
	int redtarget = 0;
	int greentarget = 0;
	int bluetarget = 0;

	// check for filename on the command line:
	if(argc < 2)
	{
		printf("You must type a filename.\n");
		exit(1);
	}
	// check for target values on the command line:
	if(argc >= 3) redtarget = atof(argv[2]);
	if(argc >= 4) greentarget = atof(argv[3]);
	if(argc >= 5) bluetarget = atof(argv[4]);

	// let user know what's going on:
	printf("Loading palette.\n");

	// load the pcx file:
	if(PcxLoadImage(argv[1], &pcx) != PCX_OK) exit(1);

	// copy palette to RGB arrays:
	for(int color = 0; color < PALETTE_SIZE; color++)
	{
		red[color] = pcx.pal[color * 3];
		green[color] = pcx.pal[color * 3 + 1];
		blue[color] = pcx.pal[color * 3 + 2];
	}

	// let user know what we're up to next:
	printf("Calculating palette tables.");

	// iterate through all 256 palette colors:
	for(color = 0; color < PALETTE_SIZE; color++)
	{
		// let user know we're still at it:
		printf(".");

		// iterate through all 32 + 1 intensity levels:
		for(int level = 0; level <= MAXLIGHT; level++)
		{
			// calculate idea lightsourced RGB values:
			float redlite = (float)(red[color] - redtarget) / MAXLIGHT * level + redtarget;
			float greenlite = (float)(green[color] - greentarget) / MAXLIGHT * level + greentarget;
			float bluelite = (float)(blue[color] - bluetarget) / MAXLIGHT * level + bluetarget;

			// initialize score to very large number:
			float bestscore = MAXFLOAT;

			// search the palette for closest match:
			for(int color2 = 0; color2 < PALETTE_SIZE; color2++)
			{
				// assign proximity score to color:
                redscore = fabs(red[color2] - redlite);
				greenscore = fabs(green[color2] - greenlite);
				bluescore = fabs(blue[color2] - bluelite);
				float score = redscore + greenscore + bluescore;

				// is this score better (i.e. smaller) than previous best?
				if(score < bestscore)
				{
					// if so, replace best score:
					bestscore = score;

					// and remember which color got it:
					litesource[level][color] = color2;
				}
			}
		}
	}

	// tell user that we're almost done:
	printf("\n\nWriting LITESORC.DAT.\n");

	// try to open disk file:
	if((file_handle = fopen("litesorc.dat", "wb")) == NULL)
	{
		perror("Error");
		exit;
	}

	// if successful, write lightsource tables to file:
	fwrite(litesource, MAXLIGHT + 1, PALETTE_SIZE, file_handle);

	// close the file handle:
	fclose(file_handle);

	// and tell the user that we're done:
	printf("\nAll done!\n");
}
