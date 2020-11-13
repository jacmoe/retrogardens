
const double PI=3.141592654;
const IMAGE_WIDTH=64;
const IMAGE_HEIGHT=64;
const MAXLIGHT=32;
const PALETTESIZE=256;
typedef int map_type[32][32];
void draw_maze(map_type map,map_type floor,map_type ceiling,
                 char far *screen,int xview,int yview,
                 long viewing_angle,int viewer_height,
                 int ambient_level,unsigned char far * textmaps,
                 unsigned char litesource[MAXLIGHT+1][PALETTESIZE],
                 unsigned char *litelevel);
