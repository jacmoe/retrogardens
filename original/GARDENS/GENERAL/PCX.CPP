#include	<stdio.h>
#include	<dos.h>
#include	"screen.h"
#include	"pcx.h"

int Pcx::load(char far *filename,pcx_struct *pcx)
{
	if ((infile=fopen(filename,"rb"))==NULL) return(-1);
	fread((char *)&(*pcx).header,sizeof(pcx_header),1,infile);
	load_image(infile,pcx);
	load_palette(infile,pcx);
	fclose(infile);
	return(0);
}

/* void Pcx::display(pcx_struct *pcx)
{
	setpalette((*pcx).palette);
	char far *screen=(char far *)MK_FP(0xa000,0);
	for(long i=0; i<64000; i++) screen[i]=(*pcx).image[i];
} */

void Pcx::load_image(FILE *pcxfile,pcx_struct *pcx)
{
	const int BYTEMODE=0, RUNMODE=1;
	int mode=BYTEMODE;
	static unsigned char outbyte,bytecount;

	pcx->image=new unsigned char[64000];
	fseek(pcxfile,128L,SEEK_SET);
	for (long i=0; i<64000; i++) {
		if (mode==BYTEMODE) {
			outbyte=(unsigned char)fgetc(pcxfile);
			if (outbyte>0xbf) {
			bytecount = (int)((int)outbyte & 0x3f);
				outbyte=(unsigned char)getc(pcxfile);
				if (--bytecount > 0) mode = RUNMODE;
			}
		}
		else if (--bytecount == 0) mode=BYTEMODE;
		(*pcx).image[i]=outbyte;
	}
}

void Pcx::load_palette(FILE *pcxfile,pcx_struct *pcx)
{
	fseek(pcxfile,-768L,SEEK_END);
	for (int i=0; i<256; i++)
		for (int j=0; j<3; j++)
			(*pcx).palette[i*3+j]=fgetc(pcxfile)>>2;
}
