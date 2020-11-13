
// WALLCAST.CPP
//
// Function to draw solid-colored walls using raycasting.
// Written by Christopher Lampton for
// GARDENS OF IMAGINATION (Waite Group, 1994).
//

#include <stdio.h>
#include <math.h>
#include "wallcast.h"

// Constant definitions:

const WALL_HEIGHT=64;           // Height of wall in pixels
const VIEWER_DISTANCE=128;      // Viewer distance from screen
const VIEWPORT_LEFT=40;         // Dimensions of viewport
const VIEWPORT_RIGHT=280;
const VIEWPORT_TOP=50;
const VIEWPORT_BOT=150;
const VIEWPORT_HEIGHT=100;
const VIEWPORT_CENTER=100;

void draw_maze(map_type map,char far *screen,int xview,
                 int yview,float viewing_angle,
                 int viewer_height)

// Draws a raycast image in the viewport of the maze represented
// in array MAP[], as seen from position XVIEW, YVIEW by a
// viewer looking at angle VIEWING_ANGLE where angle 0 is due
// north. (Angles are measured in radians.)

{
  // Variable declarations:

  int sy,offset;       // Pixel y position and offset
  float xd,yd;         // Distance to next wall in x and y
  int grid_x,grid_y;   // Coordinates of x and y grid lines
  float xcross_x,xcross_y; // Ray intersection coordinates
  float ycross_x,ycross_y;
  unsigned int xdist,ydist; // Distance to x and y grid lines
  int xmaze,ymaze;     // Map location of ray collision
  int distance;        // Distance to wall along ray

  // Loop through all columns of pixels in viewport:

  for (int column=VIEWPORT_LEFT; column<VIEWPORT_RIGHT; column++) {

    // Calculate horizontal angle of ray relative to
    //  center ray:

    float column_angle=atan((float)(column-160)
                         / VIEWER_DISTANCE);

    // Calculate angle of ray relative to maze coordinates

    float radians=viewing_angle+column_angle;

    // Rotate endpoint of ray to viewing angle:

    int x2 = 1024 * (cos(radians));
    int y2 = 1024 * (sin(radians));

    // Translate relative to viewer's position:

    x2+=xview;
    y2+=yview;

    // Initialize ray at viewer's position:

    float x=xview;
    float y=yview;

    // Find difference in x,y coordinates along ray:

    int xdiff=x2-xview;
    int ydiff=y2-yview;

    // Cheat to avoid divide-by-zero error:

    if (xdiff==0) xdiff=1;

    // Get slope of ray:

    float slope = (float)ydiff/xdiff;

    // Cheat (again) to avoid divide-by-zero error:

    if (slope==0.0) slope=.0001;

    // Cast ray from grid line to grid line:

    for (;;) {

      // If ray direction positive in x, get next x grid line:

      if (xdiff>0) grid_x=((int)x & 0xffc0)+64;

      // If ray direction negative in x, get last x grid line:

      else grid_x=((int)x & 0xffc0) - 1;

      // If ray direction positive in y, get next y grid line:

      if (ydiff>0) grid_y=((int)y & 0xffc0) +64;

      // If ray direction negative in y, get last y grid line:

      else grid_y=((int)y & 0xffc0) - 1;

      // Get x,y coordinates where ray crosses x grid line:

      xcross_x=grid_x;
      xcross_y=y+slope*(grid_x-x);

      // Get x,y coordinates where ray crosses y grid line:

      ycross_x=x+(grid_y-y)/slope;
      ycross_y=grid_y;

      // Get distance to x grid line:

      xd=xcross_x-x;
      yd=xcross_y-y;
      xdist=sqrt(xd*xd+yd*yd);

      // Get distance to y grid line:

      xd=ycross_x-x;
      yd=ycross_y-y;
      ydist=sqrt(xd*xd+yd*yd);

      // If x grid line is closer...

      if (xdist<ydist) {

        // Calculate maze grid coordinates of square:

        xmaze=xcross_x/64;
        ymaze=xcross_y/64;

        // Set x and y to point of ray intersection:

        x=xcross_x;
        y=xcross_y;

        // Is there a maze cube here? If so, stop looping:

        if (map[xmaze][ymaze]) break;
      }
      else { // If y grid line is closer:

        // Calculate maze grid coordinates of square:

        xmaze=ycross_x/64;
        ymaze=ycross_y/64;

        // Set x and y to point of ray intersection:

        x=ycross_x;
        y=ycross_y;

        // Is there a maze cube here? If so, stop looping:

        if (map[xmaze][ymaze]) break;
      }
    }

    // Get distance from viewer to intersection point:

    xd=x-xview;
    yd=y-yview;
    distance=(long)sqrt(xd*xd+yd*yd)*cos(column_angle);
    if (distance==0) distance=1;

    // Calculate visible height of wall:


    int height = VIEWER_DISTANCE * WALL_HEIGHT / distance;

    // Calculate bottom of wall on screen:

    int bot = VIEWER_DISTANCE * viewer_height
               / distance + VIEWPORT_CENTER;

    // Calculate top of wall on screen:

    int top = bot - height;

    // Clip wall to viewport:

    if (top < VIEWPORT_TOP) {
      height -= (VIEWPORT_TOP - top);
      top = VIEWPORT_TOP;
    }
    if ((top + height) > VIEWPORT_BOT) {
      height -= (bot - VIEWPORT_BOT);
    }

    // Find video offset of top pixel in wall column:

    offset = top * 320 + column;

    // Loop through all pixels in wall column:

    for (int i=0; i<height; i++) {

      // Set wall pixels to white:

      screen[offset]=15;

      // Advance to next vertical pixel:

      offset+=320;
    }
  }
}
