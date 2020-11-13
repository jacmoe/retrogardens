
// PCX.CPP
//  A set of functions for loading and compressing PCX images.
//
// Written August 7, 1993 by Christopher Lampton
// for GARDENS OF IMAGINATION (Waite Group Press)

#include	<stdio.h>
#include	<dos.h>
#include  <io.h>
#include	<fcntl.h>
#include	<mem.h>

#ifndef __PCX__
	 #include "pcx.h"
#endif

const int  MINIMUM_RUN=4;

int load_image(int pcxfile,pcx_struct *pcx);
void load_palette(int pcxfile,pcx_struct *pcx);

static int infile;

int loadPCX(char far *filename,pcx_struct *pcx)
{
  if ((infile=open(filename,O_BINARY))==-1) return(-1);
  lseek(infile,0L,SEEK_SET);
  read(infile,&(pcx->header),sizeof(pcx_header));
  if (pcx->header.xmax - pcx->header.xmin+1 > 320) return(-1);
  if (pcx->header.ymax - pcx->header.ymin+1 > 200) return(-1);
  if (pcx->header.version != 5) return(-1);
  if (load_image(infile,pcx)) return(-1);
  load_palette(infile,pcx);
  close(infile);
  return(0);
}

int load_image(int pcxfile,pcx_struct *pcx)
{
  const int BYTEMODE=0, RUNMODE=1,BUFLEN=5*1024;
  int mode=BYTEMODE,readlen;
  static unsigned char outbyte,bytecount;
  static unsigned char buffer[BUFLEN];

  int image_width=pcx->header.xmax - pcx->header.xmin + 1;
  int image_height=pcx->header.ymax - pcx->header.ymin + 1;
  long image_size=(long)image_width * image_height;
  if ((pcx->image=new unsigned char[image_size])==NULL)
       return(-1);
  int bufptr=0;
  readlen=0;
  unsigned char *image_ptr=pcx->image;
  for (long i=0; i<image_size; i++) {
    if (mode==BYTEMODE) {
      if (bufptr>=readlen) {
        bufptr=0;
        if ((readlen=read(pcxfile,buffer,BUFLEN))==0) break;
      }
      outbyte=buffer[bufptr++];
      if (outbyte>0xbf) {
        bytecount = (int)((int)outbyte & 0x3f);
        if (bufptr>=readlen) {
          bufptr=0;
          if ((readlen=read(pcxfile,buffer,BUFLEN))==0) break;
        }
        outbyte=buffer[bufptr++];
        if (--bytecount > 0) mode = RUNMODE;
      }
    }
    else if (--bytecount == 0) mode=BYTEMODE;
    *image_ptr++=outbyte;
  }
  return 0;
}

void load_palette(int pcxfile,pcx_struct *pcx)
{
  lseek(pcxfile,-768L,SEEK_END);
  read(pcxfile,pcx->palette,3*256);
  for (int i=0; i<256; i++)
    for (int j=0; j<3; j++)
      pcx->palette[i*3+j]=pcx->palette[i*3+j]>>2;
}

int compress(pcx_struct *pcx)
{
  int value,runlength,runptr;

  int image_width=pcx->header.xmax - pcx->header.xmin + 1;
  int image_height=pcx->header.ymax - pcx->header.ymin + 1;
  long image_size=(long)image_width * image_height;
  long ptr=0;
  long cptr=0;
  unsigned char *cbuffer=new unsigned char[65530];
  while (ptr<image_size) {
    if (pcx->image[ptr]==0) {
      runlength=0;
      value=pcx->image[ptr];
      while (pcx->image[ptr]==0) {
        ptr++;
        runlength++;
        if (runlength>=127) break;
        if (ptr >= image_size) break;
			}
			cbuffer[cptr] = (unsigned char) (runlength + 128);
			cptr++;
			cbuffer[cptr] = (unsigned char) value;
			cptr++;
		}
		else {
			runlength=0;
			runptr=cptr++;
			while (pcx->image[ptr]!=0) {
				cbuffer[cptr]=pcx->image[ptr];
				cptr++;
				ptr++;
				runlength++;
				if (runlength>=127) break;
				if (ptr >= image_size) break;
			}
			cbuffer[runptr]=runlength;
		}
	}
	if ((pcx->cimage=new unsigned char[cptr]) != NULL)
        {
    	memcpy(pcx->cimage,cbuffer,cptr);
	    delete cbuffer;
	    pcx->clength = cptr;
        }
    else
        cptr = 0;
	return(cptr);
}
