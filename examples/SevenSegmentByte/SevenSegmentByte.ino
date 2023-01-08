/*
  DonutStudioSevenSegment.h - Library for controlling a seven-segment-display with n digits.
  Created by Donut Studio, January 08, 2023.
  Released into the public domain.
*/

// include the libraray
#include "DonutStudioSevenSegment.h"

// define the ground pins
int gnd[] = { 10, 11, 12, 13 };

// create an object of the SegmentController class and define the segment, ground pins and display length.
SegmentController disp = SegmentController(2, 3, 4, 5, 6, 7, 8, 9, gnd, 4);

// byte for displaying (if you are using a common anode, inverse the bytes first)
byte b[4] = { B00001001, B00001001, B00001001, B00001001 };

void setup() 
{
  // initialize the display (common anode) with a refresh time of 2 and brightness of 175
  disp.initialize(true, 2, 175);

  // inverse the array if you are using common anode
  for (int i = 0; i < sizeof(b) / sizeof(b[0]); i++)
    b[i] = disp.inverseByte(b[i]);
  

  // set the fourth byte to a minus
  b[3] = disp.getMinus(); 
  // activate the dot segment(7th element) at the third byte
  b[2] = disp.setSegment(b[2], 7, true);
  // inverse the second byte
  b[1] = disp.inverseByte(b[1]);
  // addes the byte on the first byte and therefore activates them: g,f,b
  b[0] = disp.addByte(b[0], B10011101);


  // display the byte at the display
  disp.setByte(b);
}
void loop() 
{
  // refresh the display to display the byte
  disp.refreshDisplay();
}