extern "C" {
  void drawwall(char far *screenptr,char far *bitmapptr,
                 int height,long increment,char far *litelevel);
  void drawfloorrow(int row, unsigned char far *screen,
                    unsigned char far *texturelist[],
                    unsigned char far *floor,
                    unsigned char far *litelevel,
                    unsigned char far *bottoms,
                    long fix_x_increment,long fix_y_increment,
                    long fix_x,long fix_y,int width);
  void drawceilrow(int row,unsigned char far *screen,
                    unsigned char far *texturelist[],
                    unsigned char far *ceiling,
                    unsigned char far *litelevel,
                    unsigned char far *tops,
                    long fix_x_increment,long fix_y_increment,
                    long fix_x,long fix_y,int width);
}
