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
  disp.setInt(420); // will display ' 420'    identical to disp.setInt(420, false);
  //disp.setInt(420, true); // will display '0420'
  //disp.setInt(-2); // will display '  -2'
  //disp.setInt(-2, true); // will display '-002';
}
void loop() 
{
  // refresh the display in the loop
  disp.refresh();
}