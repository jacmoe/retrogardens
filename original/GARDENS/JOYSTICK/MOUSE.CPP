
// MOUSE.CPP
//
// Demonstrates mouse interface functions
//
// Written August 6, 1993 by Christopher Lampton
//  for GARDENS OF IMAGINATION (Waite Group Press)

#include	<stdio.h>
#include  <stdlib.h>
#include  <conio.h>
#include	"io.h"

void main(void)
{
  int bstatus,xrel,yrel;
  int mousex=0,mousey=0; // Mouse x,y positions

  // Clear text display:

  clrscr();

  // Initialize mouse driver:

  if (!initmouse()) exit(1);

  // Read mouse button status:

  bstatus=readmbutton();

  // Loop until right mouse button is pressed:

  while(!(bstatus & RMBUTTON)) {

    // Read relative mouse position:

    relpos(&xrel,&yrel);

    // Update mouse position variables:

    mousex+=xrel;
    mousey+=yrel;

    // Write mouse positions on display:

    gotoxy(20,10);
    printf("Mouse X=%6d",mousex);
    gotoxy(20,11);
    printf("Mouse Y=%6d",mousey);

    // Read mouse button status:

    bstatus=readmbutton();

    // Write button status on display
    gotoxy(20,12);
    if (bstatus & LMBUTTON) printf("Left button pressed    ");
    else printf("Left button not pressed");
    gotoxy(20,13);
    if (bstatus & RMBUTTON) printf("Right button pressed    ");
    else printf("Right button not pressed");
  }
}
