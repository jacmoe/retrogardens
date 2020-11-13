
void grab(int x1,int y1,int width,int height,int swidth,
           unsigned char far *screen,
           unsigned char far *buffer);
// void grab188(int x1,int y1,int width,int height,unsigned char far *screen,
//           unsigned char far *buffer);
void blit(int x1,int y1,int width,int height,unsigned char far *screen,
           unsigned char far *buffer);
extern "C" {
  void putscale(int,int,int,int,int,char far *,char far *);
}
