
#include	<stdio.h>

void linedraw(int x1,int y1,int x2,int y2,int color,
               char far *screen)
{
  int y_unit,x_unit; // Variables for amount of change
                     //  in x and y

  int offset=y1*320+x1; // Calculate offset into video RAM

  int ydiff=y2-y1;   // Calculate difference between
                     //  y coordinates
  if (ydiff<0) {     // If the line moves in the negative
                     //  direction
    ydiff=-ydiff;    // ...get absolute value of difference
    y_unit=-320;     // ...and set negative unit in
                     //  y dimension
  }
  else y_unit=320;   // Else set positive unit in
                     //  y dimension

  int xdiff=x2-x1;			// Calculate difference between
                        //  x coordinates
  if (xdiff<0) {				// If the line moves in the
                        //  negative direction
	  xdiff=-xdiff;				// ...get absolute value of
                        //  difference
	  x_unit=-1;					// ...and set negative unit
                        //  in x dimension
  }
  else x_unit=1;				// Else set positive unit in
                        //  y dimension

  int error_term=0;			// Initialize error term
  if (xdiff>ydiff) {		// If difference is bigger in
                        //  x dimension
	  int length=xdiff+1;	// ...prepare to count off in
                        //  x direction
    for (int i=0; i<length; i++) {  // Loop through points
                                    //  in x direction
      screen[offset]=color;	// Set the next pixel in the
                            //  line to COLOR
      offset+=x_unit;				// Move offset to next pixel
                            //  in x direction
      error_term+=ydiff;		// Check to see if move
                            // required in y direction
      if (error_term>xdiff) {	// If so...
        error_term-=xdiff;		// ...reset error term
        offset+=y_unit;				// ...and move offset to next
                              //  pixel in y dir.
		  }
	  }
  }
  else {								// If difference is bigger in
                        //  y dimension
    int length=ydiff+1;	// ...prepare to count off in
                        //  y direction
    for (int i=0; i<length; i++) {	// Loop through points
                                    //  in y direction
      screen[offset]=color;	// Set the next pixel in the
                            //  line to COLOR
      offset+=y_unit;				// Move offset to next pixel
                            //  in y direction
      error_term+=xdiff;    // Check to see if move
                            //  required in x direction
      if (error_term>0) {		// If so...
        error_term-=ydiff;	// ...reset error term
        offset+=x_unit;			// ...and move offset to next
                            //  pixel in x dir.
      }
    }
  }
}
