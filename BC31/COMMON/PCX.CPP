
// PCX.CPP
//  A set of functions defining a PCX object for loading
// and compressing PCX images.
//
// Copyright 1993 by Christopher Lampton and The Waite Group Press

#include	<stdio.h>
#include	<dos.h>
#include  <io.h>
#include	<fcntl.h>
#include	<mem.h>

#ifndef __TYPES__
	 #include "types.h"
#endif
#ifndef __SCREEN__
	 #include "screen.h"
#endif
#ifndef __PCX__
	 #include "pcx.h"
#endif

const long IMAGE_SIZE=64000;
const int  MINIMUM_RUN=4;

int Pcx::load(char far *filename,pcx_struct *pcx)
{
	if ((infile=open(filename,O_BINARY))==NULL) return(-1);
	lseek(infile,0L,SEEK_SET);
//	setvbuf(infile,NULL,_IOFBF,4096);
	int readlen=read(infile,&(pcx->header),sizeof(pcx_header));
//	fread((char *)&(*pcx).header,sizeof(pcx_header),1,infile);
	load_image(infile,pcx);
	load_palette(infile,pcx);
	close(infile);
//	compress(pcx);
	return(0);
}

void Pcx::load_image(int pcxfile,pcx_struct *pcx)
{
	const int BYTEMODE=0, RUNMODE=1,BUFLEN=5*1024;
	int mode=BYTEMODE,readlen;
	static unsigned char outbyte,bytecount;
	static unsigned char buffer[BUFLEN];

	pcx->image=new unsigned char[IMAGE_SIZE];

	int bufptr=0;
	readlen=0;
	unsigned char *image_ptr=pcx->image;
	for (long i=0; i<IMAGE_SIZE; i++) {
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
}

void Pcx::load_palette(int pcxfile,pcx_struct *pcx)
{
	lseek(pcxfile,-768L,SEEK_END);
	read(pcxfile,pcx->palette,3*256);
	for (int i=0; i<256; i++)
		for (int j=0; j<3; j++)
			pcx->palette[i*3+j]=pcx->palette[i*3+j]>>2;
}

int Pcx::compress(pcx_struct *pcx)
{
	int value,runlength,runptr;

	long ptr=0;
	long cptr=0;
	unsigned char *cbuffer=new unsigned char[65530];
	while (ptr<IMAGE_SIZE) {
		if (pcx->image[ptr]==0) {
			runlength=0;
			value=pcx->image[ptr];
			while (pcx->image[ptr]==0) {
				ptr++;
				runlength++;
				if (runlength>=127) break;
				if (ptr >= IMAGE_SIZE) break;
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
				if (ptr >= IMAGE_SIZE) break;
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

int Pcx::start_of_run(unsigned char *image,long ptr)
{
	if (image[ptr]!=0) return(0);
	if ((ptr+MINIMUM_RUN) >= IMAGE_SIZE) return(0);
	int runflag=1;
	int prevbyte=image[ptr];
	for (int i=1; i<MINIMUM_RUN; i++) {
		if (image[ptr+i]!=prevbyte) {
			runflag=0;
			break;
		}
	}
	return(runflag);
}
