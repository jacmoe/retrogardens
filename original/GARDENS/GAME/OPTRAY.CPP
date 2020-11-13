
// FINALRAY.CPP
//
// Function to draw lightsourced walls, floors and
// ceilings using raycasting.
// Written by Christopher Lampton for
// GARDENS OF IMAGINATION (Waite Group, 1994).
//

#include <stdio.h>
#include <math.h>
#include "optray.h"
#include "gardens.h"
#include "pcx.h"
#include "dos.h"
#include "ray.h"
#include "trig.h"
#include "fixpoint.h"
#include "objdraw.h"

// External variables:

extern unsigned char far *texturelist[NUMTEXTURES];
extern objtype obj[NUMOBJECTS];
extern map_type objmap;

int getdistance(long degrees,int column_angle,long x,long y,
                 int xview,int yview);

unsigned char bottoms[320],tops[320];
extern int objptr[16][16];
unsigned int dist[320];
int visible[16][16];
int objlist[MAXOBJECTS];

void draw_maze(map_type map,map_type floor,map_type ceiling,
                 char far *screen,int xview,int yview,
                 long viewing_angle,int viewer_height,
                 int ambient_level,unsigned char far * textmaps,
                 unsigned char litesource[MAXLIGHT+1][PALETTESIZE],
                 unsigned char *litelevel)

// Draws a raycast image in the viewport of the maze represented
// in array MAP[], as seen from position XVIEW, YVIEW by a
// viewer looking at angle VIEWING_ANGLE where angle 0 is due
// north.

{
  // Variable declarations:

  int sy,offset;       // Pixel y position and offset
  long fix_xd,fix_yd;         // Distance to next wall in x and y
  int grid_x,grid_y;   // Coordinates of x and y grid lines
  long fix_xcross_x,fix_xcross_y; // Ray intersection coordinates
  long fix_ycross_x,fix_ycross_y;
  unsigned int xdist,ydist; // Distance to x and y grid lines
  int xmaze,ymaze;     // Map location of ray collision
  int distance;  // Distance to wall along ray
  int tmcolumn;        // Column in texture map
  int top,bot;
  long fix_yratio;
  int degrees,column_angle;

  for (int ysquare=0; ysquare<16; ysquare++)
    for (int xsquare=0; xsquare<16; xsquare++)
      visible[xsquare][ysquare]=0;

  // Loop through all columns of pixels in viewport:

  for (int column=VIEWPORT_LEFT; column<=VIEWPORT_RIGHT; column++) {

    // Calculate horizontal angle of ray relative to
    //  center ray:

    column_angle=atan((float)(column-160)
                / VIEWER_DISTANCE)*(NUMBER_OF_DEGREES/6.28);
    if (column_angle<0) column_angle+=NUMBER_OF_DEGREES;
    if (column_angle>NUMBER_OF_DEGREES-1) column_angle-=NUMBER_OF_DEGREES;

    // Calculate angle of ray relative to maze coordinates

    degrees=viewing_angle+column_angle;
    if (degrees>NUMBER_OF_DEGREES-1) degrees-=NUMBER_OF_DEGREES;

    // Rotate endpoint of ray to viewing angle:

    int x2 = fixmul(-256L<<SHIFT,SIN(degrees))>>SHIFT;
    int y2 = fixmul(256L<<SHIFT,COS(degrees))>>SHIFT;

    // Translate relative to viewer's position:

    x2+=xview;
    y2+=yview;

    // Initialize ray at viewer's position:

    long fix_x=(long)xview<<SHIFT;
    long fix_y=(long)yview<<SHIFT;

    // Find difference in x,y coordinates along ray:

    long xdiff=x2-xview;
    long ydiff=y2-yview;

    // Cheat to avoid divide-by-zero error:

    if (xdiff==0) xdiff=1;
    if (ydiff==0) ydiff=1;

    // Get slope of ray:

    long fix_slope = fixdiv((long)ydiff<<SHIFT,(long)xdiff<<SHIFT);

    // Cast ray from grid line to grid line:

    for (;;) {

      // If ray direction positive in x, get next x grid line:

      if (xdiff>0) grid_x=((fix_x>>SHIFT) & 0xffc0) + 64;

      // If ray direction negative in x, get last x grid line:

      else grid_x=((fix_x>>SHIFT) & 0xffc0) - 1;

      // If ray direction positive in y, get next y grid line:

      if (ydiff>0) grid_y=((fix_y>>SHIFT) & 0xffc0) + 64;

      // If ray direction negative in y, get last y grid line:

      else grid_y=((fix_y>>SHIFT) & 0xffc0) - 1;

      // Get x,y coordinates where ray crosses x grid line:

      fix_xcross_x = (long) grid_x << SHIFT;
      fix_xcross_y = fix_y + fixmul(fix_slope,((long)grid_x<<SHIFT)-fix_x);

      // Get x,y coordinates where ray crosses y grid line:

      fix_ycross_x=fix_x+fixdiv((((long)grid_y<<SHIFT)-fix_y),fix_slope);
      fix_ycross_y=(long)grid_y<<SHIFT;

      // Get distance to x grid line:

      fix_xd=fix_xcross_x-fix_x;
      fix_yd=fix_xcross_y-fix_y;


      distance=getdistance(degrees,column_angle,
         fix_xcross_x,fix_xcross_y,xview,yview);
      xdist=distance;

      // Get distance to y grid line:

      fix_xd=fix_ycross_x-fix_x;
      fix_yd=fix_ycross_y-fix_y;
      long temp_f1=fixmul(fix_xd,fix_xd);
      long temp_f2=fixmul(fix_yd,fix_yd);
      distance=getdistance(degrees,column_angle,
         fix_ycross_x,fix_ycross_y,xview,yview);
      ydist=distance;

      // If x grid line is closer...

      if (xdist<ydist) {

        // Calculate maze grid coordinates of square:

        xmaze=fix_xcross_x>>22;
        ymaze=fix_xcross_y>>22;

        // Set x and y to point of ray intersection:

        fix_x=fix_xcross_x;
        fix_y=fix_xcross_y;

        // Find relevant column of texture map:

        tmcolumn = (fix_y>>SHIFT) & 0x3f;

        // Is there a maze cube here? If so, stop looping:

        if (map[xmaze][ymaze]) break;
        else {
          visible[xmaze][ymaze] = -1;
        }
      }
      else { // If y grid line is closer:

        // Calculate maze grid coordinates of square:

        xmaze=fix_ycross_x>>22;
        ymaze=fix_ycross_y>>22;

        // Set x and y to point of ray intersection:

        fix_x=fix_ycross_x;
        fix_y=fix_ycross_y;

        // Find relevant column of texture map:

        tmcolumn = (fix_x>>SHIFT) & 0x3f;

        // Is there a maze cube here? If so, stop looping:

        if (map[xmaze][ymaze]) break;
        else {
          visible[xmaze][ymaze] = -1;
        }
      }
    }

    // Get distance from viewer to intersection point:

    distance=getdistance(degrees,column_angle,
       fix_x,fix_y,xview,yview);

    if (distance==0) distance=1;

    dist[column]=distance;

    // Calculate visible height of wall:

    int height = VIEWER_DISTANCE * WALL_HEIGHT / distance;
    if (!height) height=1;

    // Calculate bottom of wall on screen:

    bot = VIEWER_DISTANCE * viewer_height
               / distance + VERT_CENTER;

    // Calculate top of wall on screen:

    top = bot - height + 1;

    // Initialize temporary offset into texture map:

		int t=tmcolumn*320+IMAGE_HEIGHT;

    // If top of current vertical line is outside of
    // viewport, clip it:

    long dheight=height;
    long iheight=IMAGE_HEIGHT;
		long fix_yratio=fixdiv((long)WALL_HEIGHT<<SHIFT,(long)height<<SHIFT);
		if (top < VIEWPORT_TOP) {
      int diff=VIEWPORT_TOP-top;
			dheight-=diff;
      iheight -= (diff*fix_yratio)>>SHIFT;
			top=VIEWPORT_TOP;
		}
		if (bot >= VIEWPORT_BOT) {
      int diff=bot-VIEWPORT_BOT;
			dheight -= diff;
      iheight -= diff*fix_yratio>>SHIFT;
      t-=(diff*fix_yratio)>>SHIFT;
      bot=VIEWPORT_BOT;
    }

    // Save top and bottom in arrays:

    tops[column]=top;
    bottoms[column]=bot;

    // Point to video memory offset for top of line:

		offset = bot * 320 + column;

    // Initialize vertical error term for texture map:

		int tyerror=64;

    // Which graphics tile are we using?

    int tile=map[xmaze][ymaze]-1;

    // Find offset of tile and column in bitmap:

    unsigned int tileptr=(tile/5)*320*IMAGE_HEIGHT+(tile%5)
      *IMAGE_WIDTH+t;


    long fix_increment=fixdiv(iheight<<SHIFT,dheight<<SHIFT);
    int level=litelevel[distance]+ambient_level;
    if (level>MAXLIGHT) level=MAXLIGHT;
    drawwall(&(screen[offset]),&(textmaps[tileptr]),dheight,
                fix_increment,&(litesource[level][0]));
  }

    // Step through floor pixels:

    for (int row=VERT_CENTER+5; row<=VIEWPORT_BOT; row++) {

      // Calculate horizontal angle of leftmost column relative to
      //  center ray:

      int column_angle=atan((float)(VIEWPORT_LEFT-HORIZ_CENTER)
                  / VIEWER_DISTANCE)*(NUMBER_OF_DEGREES/6.28);
      if (column_angle<0) column_angle+=NUMBER_OF_DEGREES;
      if (column_angle>NUMBER_OF_DEGREES-1) column_angle-=NUMBER_OF_DEGREES;

      // Calculate angle of ray relative to maze coordinates

      int degrees=viewing_angle+column_angle;
      if (degrees>NUMBER_OF_DEGREES-1) degrees-=NUMBER_OF_DEGREES;

      // Get ratio of viewer's height to pixel height:

      long fix_ratio=fixdiv((long)viewer_height<<SHIFT,
                            (long)(row-VERT_CENTER)<<SHIFT);

      // Get distance to visible pixel:

      long fix_distance=fixdiv(fix_ratio*VIEWER_DISTANCE,COS(column_angle));

      // Rotate distance to ray angle:

      int left_x = - (fixmul(fix_distance,SIN(degrees))>>SHIFT);
      int left_y = fixmul(fix_distance,COS(degrees))>>SHIFT;

      // Translate relative to viewer coordinates:

      left_x+=xview;
      left_y+=yview;

      // Calculate horizontal angle of rightmost column relative to
      //  center ray:

      column_angle=atan((float)(VIEWPORT_RIGHT-HORIZ_CENTER)
                  / VIEWER_DISTANCE)*(NUMBER_OF_DEGREES/6.28);
      if (column_angle<0) column_angle+=NUMBER_OF_DEGREES;
      if (column_angle>NUMBER_OF_DEGREES-1) column_angle-=NUMBER_OF_DEGREES;

      // Calculate angle of ray relative to maze coordinates

      degrees=viewing_angle+column_angle;
      if (degrees>NUMBER_OF_DEGREES-1) degrees-=NUMBER_OF_DEGREES;

      // Get ratio of viewer's height to pixel height:

      fix_ratio=fixdiv((long)viewer_height<<SHIFT,(long)(row-VERT_CENTER)<<SHIFT);

      // Get distance to visible pixel:

      fix_distance=fixdiv(fix_ratio*VIEWER_DISTANCE,COS(column_angle));

      // Rotate distance to ray angle:

      int right_x = - (fixmul(fix_distance,SIN(degrees))>>SHIFT);
      int right_y = fixmul(fix_distance,COS(degrees))>>SHIFT;

      // Translate relative to viewer coordinates:

      right_x+=xview;
      right_y+=yview;

      // Calculate stepping increment:

      long fix_x_increment=fixdiv((long)(right_x-left_x)<<SHIFT,
                            (long)(VIEWPORT_RIGHT-VIEWPORT_LEFT)<<SHIFT);
      long fix_y_increment=fixdiv((long)(right_y-left_y)<<SHIFT,
                            (long)(VIEWPORT_RIGHT-VIEWPORT_LEFT)<<SHIFT);
      long fix_x=(long)left_x<<SHIFT;
      long fix_y=(long)left_y<<SHIFT;

      int level=litelevel[fix_distance>>SHIFT]+ambient_level;
      if (level>MAXLIGHT) level=MAXLIGHT;

      drawfloorrow(row,&(screen[row*320+VIEWPORT_LEFT]),
                   &(texturelist[0]),&(floor[0][0]),
                   &(litesource[level][0]),
                   &(bottoms[VIEWPORT_LEFT]),fix_x_increment,
                   fix_y_increment,fix_x,fix_y,VIEWPORT_WIDTH);
    }

    // Step through ceiling pixels:

    for (row=VERT_CENTER-5; row>=VIEWPORT_TOP; --row) {

      // Calculate horizontal angle of leftmost column relative to
      //  center ray:

      int column_angle=atan((float)(VIEWPORT_LEFT-HORIZ_CENTER)
                  / VIEWER_DISTANCE)*(NUMBER_OF_DEGREES/6.28);
      if (column_angle<0) column_angle+=NUMBER_OF_DEGREES;
      if (column_angle>NUMBER_OF_DEGREES-1) column_angle-=NUMBER_OF_DEGREES;

      // Calculate angle of ray relative to maze coordinates

      int degrees=viewing_angle+column_angle;
      if (degrees>NUMBER_OF_DEGREES-1) degrees-=NUMBER_OF_DEGREES;

      // Get ratio of viewer's height to pixel height:

      long fix_ratio=fixdiv((long)(WALL_HEIGHT-viewer_height)<<SHIFT,
                    (long)(VERT_CENTER-row)<<SHIFT);

      // Get distance to visible pixel:

      long fix_distance=fixdiv(fix_ratio*VIEWER_DISTANCE,COS(column_angle));

      // Rotate distance to ray angle:

      int left_x = - (fixmul(fix_distance,SIN(degrees))>>SHIFT);
      int left_y = fixmul(fix_distance,COS(degrees))>>SHIFT;

      // Translate relative to viewer coordinates:

      left_x+=xview;
      left_y+=yview;

      // Calculate horizontal angle of rightmost column relative to
      //  center ray:

      column_angle=atan((float)(VIEWPORT_RIGHT-HORIZ_CENTER)
                  / VIEWER_DISTANCE)*(NUMBER_OF_DEGREES/6.28);
      if (column_angle<0) column_angle+=NUMBER_OF_DEGREES;
      if (column_angle>NUMBER_OF_DEGREES-1) column_angle-=NUMBER_OF_DEGREES;

      // Calculate angle of ray relative to maze coordinates

      degrees=viewing_angle+column_angle;
      if (degrees>NUMBER_OF_DEGREES-1) degrees-=NUMBER_OF_DEGREES;

      // Get ratio of viewer's height to pixel height:

      fix_ratio=fixdiv((long)(WALL_HEIGHT-viewer_height)<<SHIFT,(long)(VERT_CENTER-row)<<SHIFT);

      // Get distance to visible pixel:

      fix_distance=fixdiv(fix_ratio*VIEWER_DISTANCE,COS(column_angle));

      // Rotate distance to ray angle:

      int right_x = - (fixmul(fix_distance,SIN(degrees))>>SHIFT);
      int right_y = fixmul(fix_distance,COS(degrees))>>SHIFT;

      // Translate relative to viewer coordinates:

      right_x+=xview;
      right_y+=yview;

      // Calculate stepping increment:

      long fix_x_increment=fixdiv((long)(right_x-left_x)<<SHIFT,
                            (long)(VIEWPORT_RIGHT-VIEWPORT_LEFT)<<SHIFT);
      long fix_y_increment=fixdiv((long)(right_y-left_y)<<SHIFT,
                            (long)(VIEWPORT_RIGHT-VIEWPORT_LEFT)<<SHIFT);
      long fix_x=(long)left_x<<SHIFT;
      long fix_y=(long)left_y<<SHIFT;

      int level=litelevel[fix_distance>>SHIFT]+ambient_level;
      if (level>MAXLIGHT) level=MAXLIGHT;

      drawceilrow(row,&(screen[row*320+VIEWPORT_LEFT]),
                  &(texturelist[0]),&(ceiling[0][0]),
                  &(litesource[level][0]),
                  &(tops[VIEWPORT_LEFT]),fix_x_increment,
                  fix_y_increment,fix_x,fix_y,VIEWPORT_WIDTH);
    }

  int numobjs=0;
  for (xsquare=0; xsquare<16; xsquare++) {
    for (ysquare=0; ysquare<16; ysquare++) {
      if (visible[xsquare][ysquare]) {
        int objnum=objmap[xsquare][ysquare];
        if (objnum) {  // If fixed object
          --objnum;
          int x=obj[objnum].mazex-xview;
          int y=obj[objnum].mazey-yview;
          int alignx=(x*COS(-viewing_angle)
                - y*SIN(-viewing_angle))>>16;
          int aligny=(x*SIN(-viewing_angle)
                + y*COS(-viewing_angle))>>16;
          if (aligny>=0) {
            distance=sqrt((long)alignx*alignx+(long)aligny*aligny);
            int projx=-(long)alignx*VIEWER_DISTANCE/distance+HORIZ_CENTER;
            obj[objnum].wdraw=obj[objnum].wbit*VIEWER_DISTANCE/
                 distance;
            obj[objnum].hdraw=obj[objnum].hbit*VIEWER_DISTANCE/
                 distance;
            obj[objnum].screenx=projx-obj[objnum].wdraw/2;
            obj[objnum].screeny=VERT_CENTER-obj[objnum].hdraw/2;
            obj[objnum].distance=distance;
            objlist[numobjs++]=objnum;
          } // End if
        } // End if
      } // End if
    } // End for
  } // End for
  if (numobjs) {
    int swap=-1;
    while (swap) {
      swap=0;
      for (int i=0; i<(numobjs-1); i++) {
        if (obj[objlist[i]].distance<obj[objlist[i+1]].distance) {
          int temp=objlist[i];
          objlist[i]=objlist[i+1];
          objlist[i+1]=temp;
          swap=-1;
        }  // End if
      }  // End for
    }  // End while
  }  // End if
  for (int i=0; i<numobjs; i++) {
    int level=litelevel[distance]+ambient_level;
    if (level>MAXLIGHT) level=MAXLIGHT;
    drawobject(&(obj[objlist[i]]),VIEWPORT_LEFT,VIEWPORT_TOP,
       VIEWPORT_RIGHT,VIEWPORT_BOT,distance,screen,
       &(litesource[level][0]));
  } // End for
}

int getdistance(long degrees,int column_angle,long x,long y,
                   int xview,int yview)
{
  long trig,fix_ny,fix_nx;
  int distance;

  int c=degrees/512;
  if (((c&3)==0)||((c&3)==3))
    distance=abs(fixdiv((y-((long)yview<<SHIFT))>>SHIFT,
     COS(degrees-1024*c*2))*COS(column_angle)>>SHIFT);
  else
    distance=abs(fixdiv((x-((long)xview<<SHIFT))>>SHIFT,
     SIN(degrees-1024*c*2))*COS(column_angle)>>SHIFT);
  return(distance);
}
