/*
  DonutStudioSevenSegment.h - Library for controlling a seven-segment-display with multiple digits.
  Created by Donut Studio, December 30, 2023.
  Released into the public domain.
*/

/*
--- seven segment display ---

       D1        D2       D3        D4        

       -A-
    |       |
    F       B
    |       |
       -G-
    |       |
    E       C
    |       |
       -D-
            - 
            dp
*/


// include the libraray
#include "DonutStudioSevenSegment.h"

// --- define the pins ---

//                 a,  b, c, d, e, f,  g, dp
int segments[] = { 8, 12, 4, 5, 3, 7, 13, 2 };
//               d1, d2, d3, d4
int digits[] = { 11, 10, 6, 9 };

// create an instance of the contoller class: display type = common anode; 4 digits, 2ms refresh time
SegmentController disp = SegmentController(true, segments, digits, 4, 2);

void setup() 
{
  disp.setInt(1234);

  disp.setDigitState(1, false); // disable the 2nd digit
  disp.setBlinking(2, true); // let the 3rd digit blink

  disp.setBlinkInterval(100); // set the blink interval

  disp.transform(-1); // move the displayed bytes to the left

  //disp.resetEffects(); // reset all effects
}
void loop() 
{
  // refresh the display in the loop
  disp.refresh();
}