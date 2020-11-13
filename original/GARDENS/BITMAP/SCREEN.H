
const SCREEN_WIDTH=320,SCREEN_HEIGHT=200;

extern "C" {
	void cls(char far *screen_adr);
	void setmode(int mode);
	void setpalette(char far *color_regs);
	void putwindow(int,int,int,int,char far *);
	void clrwin(int,int,int,int,char far *);
  void blitscreen(char far *);
  void putimage(int,int,int,int,char far *,char far *);
}
