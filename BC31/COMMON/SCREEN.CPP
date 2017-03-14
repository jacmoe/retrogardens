#include "screen.h"
#include <mem.h>

byte* VGA = (byte*)0xA0000000L; /* points to video memory */

/* skip bytes in a file */
void fskip(FILE *fp, int num_bytes)
{
   int i;
   for (i=0; i<num_bytes; i++)
      fgetc(fp);
}

/* sets the video mode */
void set_mode(byte mode)
{
	union REGS regs;
	regs.h.ah = SET_MODE;
	regs.h.al = mode;
	int86(VIDEO_INT, &regs, &regs);
}

/* resets VGA mode 0x13 to unchained mode to access all 256K of memory */
void set_unchained_mode(void)
{
	word i;
	dword *ptr=(dword *)VGA;				/* used for faster screen clearing */

	outp(SC_INDEX,  MEMORY_MODE);			/* turn off chain-4 mode */
	outp(SC_DATA,   0x06);

	outpw(SC_INDEX, ALL_PLANES);			/* set map mask to all 4 planes */

	for(i=0;i<0x4000;i++)					/* clear all 256K of memory */
		*ptr++ = 0;

	outp(CRTC_INDEX,UNDERLINE_LOCATION);	/* turn off long mode */
	outp(CRTC_DATA, 0x00);

	outp(CRTC_INDEX,MODE_CONTROL);			/* turn on byte mode */
	outp(CRTC_DATA, 0xe3);
}

/* 	switches the pages at the appropriate time and waits for the vertical retrace. */
void page_flip(word *page1, word *page2)
{
	word high_address,low_address;
	word temp;

	temp = *page1;
	*page1 = *page2;
	*page2 = temp;

	high_address = HIGH_ADDRESS | (*page1 & 0xff00);
	low_address  = LOW_ADDRESS  | (*page1 << 8);

#ifdef VERTICAL_RETRACE
	while ((inp(INPUT_STATUS_1) & DISPLAY_ENABLE));
#endif
	outpw(CRTC_INDEX, high_address);
	outpw(CRTC_INDEX, low_address);
#ifdef VERTICAL_RETRACE
	while (!(inp(INPUT_STATUS_1) & VRETRACE));
#endif
}

/* displays a memory buffer on the screen */
void show_buffer(byte *buffer)
{
#ifdef VERTICAL_RETRACE
	while ((inp(INPUT_STATUS_1) & VRETRACE));
	while (!(inp(INPUT_STATUS_1) & VRETRACE));
#endif
	memcpy(VGA, buffer, SCREEN_SIZE);
}

/* loads a bitmap file into memory */
void load_bmp(char *file, BITMAP *b)
{
	FILE *fp;
	long index;
	word num_colors;
	int x;

	/* open the file */
	if ((fp = fopen(file,"rb")) == NULL)
	{
		printf("Error opening file %s.\n",file);
		exit(1);
	}

	/* check to see if it is a valid bitmap file */
	if (fgetc(fp) != 'B' || fgetc(fp) != 'M')
	{
		fclose(fp);
		printf("%s is not a bitmap file.\n", file);
		exit(1);
	}

	/* read in the width and height of the image, and the
	number of colors used; ignore the rest */
	fskip(fp, 16);
	fread(&b->width, sizeof(word), 1, fp);
	fskip(fp, 2);
	fread(&b->height, sizeof(word), 1, fp);
	fskip(fp, 22);
	fread(&num_colors, sizeof(word), 1, fp);
	fskip(fp, 6);

	/* assume we are working with an 8-bit file */
	if (num_colors == 0) num_colors = 256;

	/* try to allocate memory */
	if ((b->data = (byte *) malloc((word)(b->width * b->height))) == NULL)
	{
		fclose(fp);
		printf("Error allocating memory for file %s.\n", file);
		exit(1);
	}

	/* read the palette information */
	for(index = 0; index < num_colors; index++)
	{
		b->palette[(int)(index * 3 + 2)] = fgetc(fp) >> 2;
		b->palette[(int)(index * 3 + 1)] = fgetc(fp) >> 2;
		b->palette[(int)(index * 3 + 0)] = fgetc(fp) >> 2;
		x=fgetc(fp);
	}

	/* read the bitmap */
	for(index = (b->height - 1) * b->width; index >= 0;index -= b->width)
		for(x = 0; x < b->width; x++)
			b->data[(int)(index + x)]=(byte)fgetc(fp);

	fclose(fp);
}

/* sets all 256 colors of the palette */
void set_palette(byte *palette)
{
	int i;

	outp(PALETTE_INDEX, 0);				/* tell the VGA that palette data is coming. */
	for(i = 0; i < 256 * 3; i++)
		outp(PALETTE_DATA, palette[i]);	/* write the data */
}

/* plots a pixel in unchained mode */
void plot_pixel_um(int x, int y, byte color)
{
	outp(SC_INDEX, MAP_MASK);	/* select plane */
	outp(SC_DATA,  1 << (x&3) );

	VGA[(y << 6) + (y << 4) + (x >> 2)] = color;
}

/* plots a pixel */
void plot_pixel(int x, int y, byte color)
{
	//VGA[y * SCREEN_WIDTH + x] = color;
	// y * 320 = y * 256 + y * 64 = y * 2^8 + y * 2^6
	VGA[(y << 8) + (y << 6) + x] = color;
}

/* plots a pixel */
void plot_pixel_buf(int x, int y, byte color, byte* buffer)
{
	//VGA[y * SCREEN_WIDTH + x] = color;
	// y * 320 = y * 256 + y * 64 = y * 2^8 + y * 2^6
	buffer[(y << 8) + (y << 6) + x] = color;
}

/* draws a line using Bresenham's line-drawing algorithm
   no multiplication or division */
void line(int x1, int y1, int x2, int y2, byte color)
{
  int i,dx,dy,sdx,sdy,dxabs,dyabs,x,y,px,py;

  dx=x2-x1;      /* the horizontal distance of the line */
  dy=y2-y1;      /* the vertical distance of the line */
  dxabs=abs(dx);
  dyabs=abs(dy);
  sdx=sgn(dx);
  sdy=sgn(dy);
  x=dyabs>>1;
  y=dxabs>>1;
  px=x1;
  py=y1;

  VGA[(py<<8)+(py<<6)+px]=color;

  if (dxabs>=dyabs) /* the line is more horizontal than vertical */
  {
	for(i=0;i<dxabs;i++)
	{
	  y+=dyabs;
	  if (y>=dxabs)
	  {
		y-=dxabs;
		py+=sdy;
	  }
	  px+=sdx;
	  plot_pixel(px,py,color);
	}
  }
  else /* the line is more vertical than horizontal */
  {
	for(i=0;i<dyabs;i++)
	{
	  x+=dxabs;
	  if (x>=dyabs)
	  {
		x-=dyabs;
		px+=sdx;
	  }
	  py+=sdy;
	  plot_pixel(px,py,color);
	}
  }
}

/* draws a line using Bresenham's line-drawing algorithm
   no multiplication or division */
void line_buf(int x1, int y1, int x2, int y2, byte color, byte* buffer)
{
  int i,dx,dy,sdx,sdy,dxabs,dyabs,x,y,px,py;

  dx=x2-x1;      /* the horizontal distance of the line */
  dy=y2-y1;      /* the vertical distance of the line */
  dxabs=abs(dx);
  dyabs=abs(dy);
  sdx=sgn(dx);
  sdy=sgn(dy);
  x=dyabs>>1;
  y=dxabs>>1;
  px=x1;
  py=y1;

  buffer[(py<<8)+(py<<6)+px]=color;

  if (dxabs>=dyabs) /* the line is more horizontal than vertical */
  {
	for(i=0;i<dxabs;i++)
	{
	  y+=dyabs;
	  if (y>=dxabs)
	  {
		y-=dxabs;
		py+=sdy;
	  }
	  px+=sdx;
	  plot_pixel_buf(px, py, color, buffer);
	}
  }
  else /* the line is more vertical than horizontal */
  {
	for(i=0;i<dyabs;i++)
	{
	  x+=dxabs;
	  if (x>=dyabs)
	  {
		x-=dyabs;
		px+=sdx;
	  }
	  py+=sdy;
	  plot_pixel_buf(px, py, color, buffer);
	}
  }
}

/* draws a rectangle */
void rect(int left, int top, int right, int bottom, byte color)
{
	word top_offset, bottom_offset, i, temp;

	if(top > bottom)
	{
		temp = top;
		top = bottom;
		bottom = temp;
	}
	if(left > right)
	{
		temp = left;
		left = right;
		right = temp;
	}

	top_offset = (top << 8) + (top << 6);
	bottom_offset = (bottom << 8) + (bottom << 6);

	for(i = left; i <= right; i++)
	{
		VGA[top_offset + i] = color;
		VGA[bottom_offset + i] = color;
	}
	for(i = top_offset; i <= bottom_offset; i += SCREEN_WIDTH)
	{
		VGA[left + i] = color;
		VGA[right + i] = color;
	}
}

/* draws a rectangle */
void rect_buf(int left, int top, int right, int bottom, byte color, byte* buffer)
{
	word top_offset, bottom_offset, i, temp;

	if(top > bottom)
	{
		temp = top;
		top = bottom;
		bottom = temp;
	}
	if(left > right)
	{
		temp = left;
		left = right;
		right = temp;
	}

	top_offset = (top << 8) + (top << 6);
	bottom_offset = (bottom << 8) + (bottom << 6);

	for(i = left; i <= right; i++)
	{
		buffer[top_offset + i] = color;
		buffer[bottom_offset + i] = color;
	}
	for(i = top_offset; i <= bottom_offset; i += SCREEN_WIDTH)
	{
		buffer[left + i] = color;
		buffer[right + i] = color;
	}
}

/* draws a filled rectangle */
void rect_fill(int left, int top, int right, int bottom, byte color)
{
	word top_offset, bottom_offset, i, temp, width;

	if(top > bottom)
	{
		temp = top;
		top = bottom;
		bottom = temp;
	}
	if(left > right)
	{
		temp = left;
		left = right;
		right = temp;
	}

	top_offset = (top << 8) + (top << 6) + left;
	bottom_offset = (bottom << 8) + (bottom << 6) + left;
	width = right - left + 1;

	for(i = top_offset; i <= bottom_offset; i += SCREEN_WIDTH)
	{
		memset(&VGA[i], color, width);
	}
}
