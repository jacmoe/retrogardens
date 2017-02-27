#ifndef __SCREEN__
#define __SCREEN__

const SCREEN_WIDTH=320,SCREEN_HEIGHT=200;

// in SCREENC.CPP

void SetGfxBuffer( void far* theBuffer );
void far *XyToPtr(int x, int y);
void WritePixel(int x, int y, char color);
void GetImage(int x1, int y1, int x2, int y2, char *buffer);
void PutImage(int x1, int y1, int x2, int y2, char *buffer);
void ReadPalette(int start, int number, char* palette);
void ClrPalette(int start, int number);
void WaitVbi();
void ClearScr(char color);
void BarFill(int x1, int y1, int x2, int y2, char color);
void Line(int x1, int y1, int x2, int y2, char color);
int BufSize( int x1, int y1, int x2, int y2 );

// in SCREEN.ASM

extern "C" {
	void cls(char far *screen_adr);
	void setgmode(int mode);
	void setpalette(char far *color_regs,int firstreg,int numregs);
	void putwindow(int,int,int,int,char far *);
	void clrwin(int,int,int,int,char far *);
  void blitscreen( void far* buffer );
  void putimage( int x1, int y1, int xdim, int ydim, void far* sbuf,
  void far* dbuf);
	void ctransput(void far *sbuffer, void far *dbuffer);
  void transput( int x1, int y1, int x2, int y2, void far* sbuffer,
  void far* dbuffer );
  void fadepalin(int start, int count, const char* palette);
  void fadepalout(int start, int count);
  int detectvga();
}

#endif