//
// FLOORCST.CPP
// Draws texture mapped floor
// Written by Christopher Lampton
// for Gardens of Imagination (Waite Group, 1994)

#include <stdio.h>
#include <math.h>
#include "floorcst.h"

const WALL_HEIGHT=64;           // Height of wall in pixels
const VIEWER_DISTANCE=128;      // Viewer distance from screen
const VIEWPORT_LEFT=0;          // Dimensions of viewport
const VIEWPORT_RIGHT=319;
const VIEWPORT_TOP=0;
const VIEWPORT_BOT=199;
const VIEWPORT_HEIGHT=VIEWPORT_BOT-VIEWPORT_TOP;
const VIEWPORT_CENTER=VIEWPORT_TOP+VIEWPORT_HEIGHT/2;
const ZOOM=2;


void draw_maze(map_type floor,map_type ceiling,char far *screen,
                       int xview,int yview,
                       float viewing_angle,int viewer_height,
                       char far *textmaps)
{
  int y_unit,x_unit; // Variables for amount of change
                     //  in x and y
  int distance,old_distance,xd,yd,sx,sy,offset;

  // Loop through all columns of pixels in viewport:

  for (int column=VIEWPORT_LEFT; column<VIEWPORT_RIGHT; column++) {

    // Calculate horizontal angle of ray relative to
    //  center ray:

    float column_angle=atan((float)((column-160)/ZOOM)
                         / VIEWER_DISTANCE);

    // Calculate angle of ray relative to maze coordinates

    float radians=viewing_angle+column_angle;

    // Step through floor and ceiling pixels:

    for (int row=VIEWPORT_BOT; row>VIEWPORT_CENTER+5; --row) {

      // Get ratio of viewer's height to pixel height:

      float ratio=(float)viewer_height/(row-VIEWPORT_CENTER);

      // Get distance to visible pixel:

      distance=ratio*VIEWER_DISTANCE/cos(column_angle);

      // Rotate distance to ray angle:

      int x = - distance * (sin(radians));
      int y = distance * (cos(radians));

      // Translate relative to viewer coordinates:

      x+=xview;
      y+=yview;

      // Get maze square intersected by ray:

      int xmaze = x / 64;
      int ymaze = y / 64;

      // Find relevant column of texture map:

      int t = ((int)y & 0x3f) * 320 + ((int)x & 0x3f);

      // Which graphics tile are we using?

      int tile=floor[xmaze][ymaze];

      // Find offset of tile and column in bitmap:

      unsigned int tileptr=(tile/5)*320*IMAGE_HEIGHT+(tile%5)
        *IMAGE_WIDTH+t;

      // Calculate video offset of floor pixel:

      offset=row*320+column;

      // Draw pixel:

      screen[offset]=textmaps[tileptr];
    }

    // Step through ceiling pixels:

    for (row=VIEWPORT_TOP; row<VIEWPORT_CENTER-5; row++) {

      // Get ratio of viewer's height to pixel height:

      float ratio=(float)(WALL_HEIGHT-viewer_height)
                    / (VIEWPORT_CENTER-row);

      // Get distance to visible pixel:

      distance=ratio*VIEWER_DISTANCE/cos(column_angle);

      // Rotate distance to ray angle:

      int x = - distance * (sin(radians));
      int y = distance * (cos(radians));

      // Translate relative to viewer coordinates:

      x+=xview;
      y+=yview;

      // Get maze square intersected by ray:

      int xmaze = x / 64;
      int ymaze = y / 64;

      // Find relevant column of texture map:

      int t = ((int)y & 0x3f) * 320 + ((int)x & 0x3f);

      // Which graphics tile are we using?

      int tile=ceiling[xmaze][ymaze];

      // Find offset of tile and column in bitmap:

      unsigned int tileptr=(tile/5)*320*IMAGE_HEIGHT+(tile%5)
        *IMAGE_WIDTH+t;

      // Calculate video offset of floor pixel:

      offset=row*320+column;

      // Draw pixel:

      screen[offset]=textmaps[tileptr];
    }
  }
}
