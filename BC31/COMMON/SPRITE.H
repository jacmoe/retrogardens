
// Structure for sprite variables:

/* struct sprite_struc {
	char far *image;			// Pointer to array containing sprite bitmap
//	char far *savebuffer; // Pointer to array for saving sprite background
//	int	width,height;			// Width and height of sprite in pixels
//	int x,y;							// Current x,y coordinates of sprite
}; */

// Declaration of sprite class:

class Sprite
{
	private:
		char far **image;				// Pointer to sprite bitmaps
		int width,height;				// Width and height of sprite bitmap
		int xsprite,ysprite;		// Current x,y coordinates of sprite
		char far *savebuffer;		// Pointer to array for saving sprite background
	public:
		Sprite(int num_sprites,int width,int height);	// Constructor for sprite class
		void grab(unsigned char far *buffer,int sprite_num,int x1,int y1);
		void put(int sprite_num,int x,int y,unsigned char far *screen);
		void erase(unsigned char far *screen);
};

