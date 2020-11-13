
#include <stdio.h>

void grab(int x1,int y1,int width,int height,
            unsigned char far *screen,
            unsigned char far *buffer)
{
  for (int y=0; y<height; y++)
    for (int x=0; x<width; x++)
      buffer[y*width+x]=screen[(y1+y)*320+x1+x];
}

void blit(int x1,int y1,int width,int height,unsigned char far *screen,
           unsigned char far *sprite)
{
  for (int y=0; y<height; y++)
    for (int x=0; x<width; x++) {
      unsigned char byte=sprite[y*width+x];
//      if (byte) screen[(y1+y)*320+x1+x]=byte;
      screen[(y1+y)*320+x1+x]=byte;
    }
}
