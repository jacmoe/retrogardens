//
// HIGHCAST.CPP
// Draws height-mapped three-dimensional maze
// Written by Christopher Lampton
// for Gardens of Imagination (Waite Group, 1994)

#include <stdio.h>
#include <math.h>
#include "tilecast.h"
#include "pcx.h"

const WALL_HEIGHT=64;           // Height of wall in pixels
const VIEWER_DISTANCE=128;      // Viewer distance from screen
const VIEWPORT_LEFT=0;          // Dimensions of viewport
const VIEWPORT_RIGHT=319;
const VIEWPORT_TOP=0;
const VIEWPORT_BOT=199;
const VIEWPORT_HEIGHT=VIEWPORT_BOT-VIEWPORT_TOP;
const VIEWPORT_CENTER=VIEWPORT_TOP+VIEWPORT_HEIGHT/2;
const ZOOM=2;
const GRIDWIDTH=16;
const WALLCOLOR=18;

void draw_maze(map_type wall,map_type floor,
                 map_type hightile,map_type floorbase,
                 char far *screen,int xview,int yview,
                 float viewing_angle,int viewer_height,
                 char far *textmaps,char far *highmaps)
{
  int y_unit,x_unit; // Variables for amount of change
                     //  in x and y
  int distance,real_distance,old_distance,xd,yd,sx,sy;
  unsigned int offset;

  // Loop through all columns of pixels in viewport:

  for (int column=VIEWPORT_LEFT; column<VIEWPORT_RIGHT; column++) {

    // Calculate horizontal angle of ray relative to
    //  center ray:

    float column_angle=atan((float)((column-160)/ZOOM)
                         / VIEWER_DISTANCE);

    // Fudge column angle:

    if (column_angle==0.0) column_angle=0.0001;

    // Calculate angle of ray relative to maze coordinates

    float radians=viewing_angle+column_angle;

    // Which square is the viewer standing on?

    int xmaze = xview/64;
    int ymaze = yview/64;

    // Get pointer to floor height map:

    int floortile=hightile[xmaze][ymaze];

    // How high is the floor under the viewer?

    int currentheight=highmaps[(floortile/5)*320*IMAGE_HEIGHT
                 +(floortile%5)*IMAGE_WIDTH
                 +(yview%IMAGE_HEIGHT)*320
                 +(xview%IMAGE_WIDTH)];

    // First screen row to draw:

    int row=VIEWPORT_BOT;

    // Cast a ray across the floor:

    for (;;) {

      // Get ratio of viewer's height to pixel height:

      float screen_height=row-VIEWPORT_CENTER;
      if (screen_height==0.0) screen_height=.00001;
      float ratio=(float)(viewer_height-currentheight)/screen_height;

      // Get distance to pixel:

      real_distance=ratio*VIEWER_DISTANCE;
      if (real_distance>old_distance+5) distance+=5;
      distance=real_distance/cos(column_angle);

      // Rotate distance to ray angle:

      int x = - distance * (sin(radians));
      int y = distance * (cos(radians));

      // Translate relative to viewer coordinates:

      x+=xview;
      y+=yview;

      // Get maze square intersected by ray:

      int xmaze = x / 64;
      int ymaze = y / 64;

      // If we've reached the edge of the map, quit:

      if ((xmaze<0)||(xmaze>=GRIDWIDTH)
          ||(ymaze<0)||(ymaze>=GRIDWIDTH))
        break;

      // Find relevant column of texture and height maps:

      int t = ((int)y & 0x3f) * 320 + ((int)x & 0x3f);

      // Which height tile are we using?

      int tile=hightile[xmaze][ymaze];

      // Find offset of tile and column in bitmap:

      unsigned int tileptr=(tile/5)*320*IMAGE_HEIGHT+(tile%5)
        *IMAGE_WIDTH+t;

      // Get height of pixel:

      int newheight=highmaps[tileptr]+floorbase[xmaze][ymaze];

      // Has the floor level gone down?

      if (newheight < currentheight) currentheight=newheight;

      else {

        // Has the floor level gone up?

        if (newheight > currentheight) {

          // If so, set new floor height:

          currentheight=newheight;

          // If so, calculate new screen position:

          ratio = (float)VIEWER_DISTANCE/real_distance;
          int a = ratio*(viewer_height-currentheight);
          int newrow = VIEWPORT_CENTER + a;

          // Draw wall segment:

          offset=row*320+column;
          for (int i=row; i>newrow; --i) {
            screen[offset]=wall[xmaze][ymaze];
            offset-=320;
          }

          // Set screen row to new position:

          row=newrow;
        }

        // If viewer is looking down on floor, draw floor pixel:

        if (row > VIEWPORT_CENTER) {

          // Which graphics tile are we using?

          tile=floor[xmaze][ymaze];

          // Find offset of tile and column in bitmap:

          tileptr=(tile/5)*320*IMAGE_HEIGHT+(tile%5)*IMAGE_WIDTH+t;

          // Calculate video offset of floor pixel:

          offset=row*320+column;

          // Draw pixel:

          screen[offset]=textmaps[tileptr];

          // Advance to next screen line:

          row--;
        }
      }
    }
  }
}
