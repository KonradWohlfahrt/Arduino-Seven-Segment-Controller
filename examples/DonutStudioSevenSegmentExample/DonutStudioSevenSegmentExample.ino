/*
  DonutStudioSevenSegment.h - Library for controlling a seven-segment-display with n digits.
  Created by Donut Studio, December 24, 2022.
  Released into the public domain.
*/

// include the libraray
#include "DonutStudioSevenSegment.h"

// define the ground pins
int gnd[] = { 10, 11, 12, 13 };

// create an object of the SegmentController class and define the segment, ground pins and display length.
SegmentController disp = SegmentController(2, 3, 4, 5, 6, 7, 8, 9, gnd, 4);


// byte for displaying: helo
byte hello[4] = { B01011100, B00111000, B01111001, B00110110 };

void setup() 
{
  // initialize the display (common anode) with a refresh time of 2 and brightness of 175
  disp.initialize(true, 2, 175);

  ///*
  // display the number 1234 on the display. the range is: -999 - 9999
  disp.setInt(420);
  // disable the first digit
  disp.setDigit(0, false);
  // enable blinking for every digit
  disp.setBlinkAll();
  //*/

  /*
  // set the fourth byte to a minus
  hello[3] = disp.getMinus(); 
  // activate the dot segment at the third byte
  hello[2] = disp.addSegment(hello[2], 7);
  // display the byte at the display
  disp.setByte(hello);
  */
}
void loop() 
{
  // refresh the display to display the number
  disp.refreshDisplay();
}