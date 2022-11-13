/*
  DonutStudioSevenSegment.h - Library for controlling a seven-segment-display with n digits.
  Created by Donut Studio, November 13, 2022.
  Released into the public domain.
*/

#include "DonutStudioSevenSegment.h"

#define MAXDIGITS 4

int gnd[] = { 10, 11, 12, 13 };
SegmentController disp = SegmentController(2, 3, 4, 5, 6, 7, 8, 9, gnd);

void setup() 
{
  disp.initialize(true, 2, 200);

  disp.setInt(1421);
  disp.setDigit(0, false);
  disp.setDigit(3, false);
  disp.setBlinkAll(true);
}
void loop() 
{
  disp.refreshDisplay();
}
