#define	NO_ERROR 0
#define FILE_NOT_OPENED 1
#define INSUFFICIENT_MEMORY 2
#define TOO_MANY_ARGUMENTS 3

#define WORD unsigned int
#define BYTE unsigned char

struct tga_header {
  BYTE ID_length;
  BYTE color_map_type;
  BYTE image_type;
  WORD first_color_map_entry;
  WORD color_map_length;
  BYTE color_map_entry_size;
  WORD image_x_origin;
  WORD image_y_origin;
  WORD image_width;
  WORD image_height;
  BYTE bits_per_pixel;
  BYTE image_descriptor_bits;
};

struct tga_struct {
  tga_header header;
  char far *ID;
  BYTE far *image;
  BYTE far *color_map;
};

int  loadTGA(char far *filename,tga_struct *tga);
