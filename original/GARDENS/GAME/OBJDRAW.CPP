#include <stdio.h>
#include <math.h>
#include "optray.h"
#include "objdraw.h"
#include "gardens.h"
#include "trig.h"

// Function to draw a texture mapped polygon defined by
// polytype parameter POLY clipped against a viewport with
// upper lefthand coordinates of LEFTX,LEFTY and lower
// righthand coordinates of RIGHTX,RIGHTY.

extern unsigned char bottoms[320],tops[320];
extern char far *image[NUMIMAGES];
extern unsigned int dist[320];

void drawobject(objtype *obj,int lclipx,int lclipy,int rclipx,
			 int rclipy,unsigned int distance,char *screen,
       char *litelevel)
{
  unsigned int bptr,sptr,sline;
  unsigned long line;

  char far *bitmap=image[obj->imagenum];
  long fix_wincrement=(float)obj->wbit/obj->wdraw*SHIFT_MULT;
  long fix_hincrement=(float)obj->hbit/obj->hdraw*SHIFT_MULT;
  int sx=obj->screenx;
  int sy=obj->screeny;
  unsigned long column=0;
  for (unsigned int x=0; x<obj->hdraw; x++) {
    column+=fix_wincrement;
    if ((sx >= lclipx) & (sx <= rclipx)) {
      int vert_offset=VIEWPORT_TOP-sy;
      if (vert_offset>0) {
         line=((long)vert_offset*fix_hincrement)+VIEWPORT_TOP;
         sptr=VIEWPORT_TOP*320+sx;
         bptr=(column>>16)+(((long)vert_offset*fix_hincrement)>>16)*obj->wbit;
         sline=VIEWPORT_TOP;
      }
      else {
        line=0;
        sptr=sy*320+sx;
        bptr=column>>16;
        sline=sy;
      }
      unsigned int lastline=line>>16;
      if (((sx)>=lclipx) && ((sx)<=rclipx)) {
        if (distance<dist[sx]) {
          for (unsigned int y=lastline; y<obj->wdraw; y++) {
            int b=bitmap[bptr];
            if (b&&((line>>16)<=VIEWPORT_BOT))
              screen[sptr]=litelevel[b];
            line+=fix_hincrement;
            int adv=(line>>16)-lastline;
            if (adv) {
              bptr+=obj->wbit*adv;
              lastline=line>>16;
            }
            sptr+=320;
            sline++;
            if (sline>VIEWPORT_BOT) break;
          }
        }
      }
    }
    sx++;
  }
}
