//
// [Listing 10]
//
//  Copyright (c) 1994 by Mark Seminatore, all rights reserved.
//
//  Dr. Dobb's Journal July 1994
//

//
//  Pcx.c:  This file contains the PCX file loader/decoder.
//

    #include <stdio.h>
    #include <stdlib.h>
    #include <dos.h>
    #include <mem.h>

    #include "pcx.h"

    #define IMAGE_X    320
    #define IMAGE_Y    200
    #define MAX_SIZE 64000L

    enum {BYTEMODE,RUNMODE};

    int PcxLoadImage(char *filename,PcxImage *pcx)
    {
      long i;
      int mode=BYTEMODE,bytecount;
      unsigned char abyte,*p;
      FILE *f;

      f=fopen(filename,"rb");
      if(f==NULL)
        return PCX_NOFILE;
      fread(&pcx->hdr,sizeof(PcxHeader),1,f);
      pcx->width=1+pcx->hdr.xmax-pcx->hdr.xmin;
      pcx->height=1+pcx->hdr.ymax-pcx->hdr.ymin;
      pcx->imagebytes=(unsigned int)(pcx->width*pcx->height);
      if(pcx->imagebytes > MAX_SIZE)
        return PCX_TOOBIG;
      pcx->bitmap=(unsigned char*)malloc(pcx->imagebytes);
      if(pcx->bitmap == NULL)
        return PCX_NOMEM;

      p=pcx->bitmap;
      for(i=0;i<pcx->imagebytes;i++)
      {
        if(mode == BYTEMODE)
        {
          abyte=fgetc(f);
          if(abyte > 0xbf)
          {
            bytecount=abyte & 0x3f;
            abyte=fgetc(f);
            if(--bytecount > 0)
              mode=RUNMODE;
          }
        }
        else if(--bytecount == 0)
          mode=BYTEMODE;
        *p++=abyte;
      }

      fseek(f,-768L,SEEK_END);      // get palette from pcx file
      fread(pcx->pal,768,1,f);
      p=pcx->pal;
      for(i=0;i<768;i++)            // bit shift palette
        *p++=*p >>2;
      fclose(f);
      return PCX_OK;                // return success
    }

//
//  This routine transposes a bitmap, ie. pixel (x,y) -> (y,x).
//  The texture mapping is faster if we draw vertical columns from the
//  bitmaps by copying sequential bytes from the bitmap arrays.  Otherwise
//  we would have to add 64 inside a loop.
//
    int TransposeBitmap(char *bmp,int w,int h)
    {
	  char* p;
	  int i,j;

	  p = (char*)malloc(w*h*sizeof(char));
      if(p==NULL)
        return 0;
      memcpy(p,bmp,w*h);
      for(i=0;i<w;i++)
        for(j=0;j<h;j++)
          bmp[i*h+j]=p[j*w+i];
      free(p);
      return 1;
    }
