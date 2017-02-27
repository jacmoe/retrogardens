#ifndef __PCX__                  // added this for build protection
#define __PCX__

#define	NO_ERROR 0
#define FILE_NOT_OPENED 1
#define INSUFFICIENT_MEMORY 2
#define TOO_MANY_ARGUMENTS 3

struct pcx_header {
	char manufacturer;
	char version;
	char encoding;
	char bits_per_pixel;
	int  xmin,ymin;
	int  xmax,ymax;
	int  hres;
	int  vres;
	char palette16[48];
	char reserved;
	char color_planes;
	int  bytes_per_line;
	int  palette_type;
	char filler[58];
};


struct pcx_struct {
	pcx_header header;
	unsigned char far *image;
	unsigned char far *cimage;
	unsigned char palette[3*256];
	int clength;
};

class Pcx
{
	private:
		int infile;
		void load_image(int pcxfile,pcx_struct *pcx);
		void load_palette(int pcxfile,pcx_struct *pcx);
		int start_of_run(unsigned char * image,long ptr);
	public:
		int load(char far *filename,pcx_struct *pcx);
		int compress(pcx_struct *pcx);
};

#endif