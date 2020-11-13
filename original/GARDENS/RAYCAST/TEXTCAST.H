
const double PI=3.141592654;
const IMAGE_WIDTH=64;
const IMAGE_HEIGHT=64;
typedef int map_type[16][16];
void draw_maze(map_type map,char far *screen,int xview,int yview,
                       float viewing_angle,int viewer_height,
                       char far *textmaps);
