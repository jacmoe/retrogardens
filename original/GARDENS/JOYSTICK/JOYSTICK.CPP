
// JOYSTICK.CPP
//
// Demonstrates joystick interface functions
//
// Written August 6, 1993 by Christopher Lampton
//  for GARDENS OF IMAGINATION (Waite Group Press)

#include	<stdio.h>
#include  <stdlib.h>
#include  <conio.h>
#include	"io.h"

void main(void)
{
  int xstick,ystick;
  int xcent,ycent;
  int status1,status2;

  // Clear the text display:

  clrscr();

  // Calibrate the user's joystick:

  printf("\n\nCenter your joystick and press button ");
  printf("one.\n");
  while (!readjbutton(JBUTTON1)); // Loop until joystick
                                  //  button pressed
  for (long i=1; i<100000; i++);  // Brief delay
  xcent=readstick(JOY_X);         // Get x coordinate
  ycent=readstick(JOY_Y);         // Get y coordinate
  while (readjbutton(JBUTTON1));  // Loop until button
                                  //  released
  // Set up display:

  clrscr();
  gotoxy(20,8);
  printf("Center x position at %d",xcent);
  gotoxy(20,9);
  printf("Center y position at %d",ycent);
  gotoxy(20,10);
  printf("X axis position:");
  gotoxy(20,12);
  printf("Y axis position:");

  // Read button 2 status:

  status2=readjbutton(JBUTTON2);

  // Loop until right mouse button is pressed:

  while(!status2) {

    // Read X and Y axis positions:

    xstick=readstick(JOY_X);
    ystick=readstick(JOY_Y);

    // Write joystick positions on display:

    gotoxy(20,11);
    if (xstick<xcent) printf("  Left of center ");
    if (xstick>xcent) printf("  Right of center");
    if (xstick==xcent) printf("  Centered       ");
    printf(" (%d)    ",xstick);
    gotoxy(20,13);
    if (ystick<ycent) printf("  Above center");
    if (ystick>ycent) printf("  Below center");
    if (ystick==ycent) printf("  Centered    ");
    printf(" (%d)   ",ystick);

    // Read mouse button status:

    status1=readjbutton(JBUTTON1);
    status2=readjbutton(JBUTTON2);

    // Write button status on display

    gotoxy(20,14);
    if (status1) printf("Button 1 pressed    ");
    else printf("Button 1 not pressed");
    gotoxy(20,15);
    if (status2) printf("Button 2 pressed    ");
    else printf("Button 2 not pressed");
  }
}
