//
// [Listing 3]
//
//  Copyright (c) 1994 by Mark Seminatore, all rights reserved.
//
//  Dr. Dobb's Journal July 1994
//

//
//  This header file declares the PCX file functions
//

#ifndef __PCX_H
  #define __PCX_H

    typedef struct
    {
      char manufacturer;    /* Always set to 0 */
      char version;         /* Always 5 for 256-color files */
      char encoding;        /* Always set to 1 */
      char bits_per_pixel;  /* Should be 8 for 256-color files */
      int  xmin,ymin;       /* Coordinates for top left corner */
      int  xmax,ymax;       /* Width and height of image */
      int  hres;            /* Horizontal resolution of image */
      int  vres;            /* Vertical resolution of image */
      char palette16[48];   /* EGA palette; not used for 256-color files */
      char reserved;        /* Reserved for future use */
      char color_planes;    /* Color planes */
      int  bytes_per_line;  /* Number of bytes in 1 line of pixels */
      int  palette_type;    /* Should be 2 for color palette */
      char filler[58];      /* Nothing but junk */
    } PcxHeader;

    typedef struct
    {
      PcxHeader hdr;
      char *bitmap;
      unsigned char pal[768];
      unsigned imagebytes,width,height;
    } PcxImage;

    enum {PCX_OK,PCX_NOMEM,PCX_TOOBIG,PCX_NOFILE};

#ifdef __cplusplus
extern "C" {
#endif

    int PcxLoadImage(char *filename,PcxImage *pcx);
    int TransposeBitmap(char*,int,int);
#ifdef __cplusplus
  }
#endif

#endif
