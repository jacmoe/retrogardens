
// SCANKEY.CPP
//
// Demonstrates alternate keyboard handler
//
// Written August 5, 1993 by Christopher Lampton
//  for GARDENS OF IMAGINATION (Waite Group Press)

#include	<stdio.h>
#include	"io.h"

char far scanbuffer[128]; // Buffer for 128 scan codes

void main(void)
{
  // Install alternate keyboard driver:

  initkey(scanbuffer);

  // Loop until ESC (scan code 1) is pressed:

  while(!scanbuffer[1]) {

    // Watch for right SHIFT key:

    if (scanbuffer[54]) printf("SHIFT\n");

    // Watch for ENTER key:

    if (scanbuffer[28]) printf("ENTER\n");
  }

  // Remove alternate keyboard handler:

  remkey();
}
