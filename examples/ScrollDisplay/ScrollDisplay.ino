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


// a custom array of bytes to be displayed
byte b[4] = { 0b00001001, 0b00100010, 0b10011100, 0b00110001 };


void setup() 
{
  Serial.begin(9600);

  disp.setScroller("Hey, text!"); // display 'hey, text!' on the device
  disp.appendScroller(b, 4); // add some custom bytes to the scroll effect
  
  disp.setScrollerUpdateTime(500); // move to the next char in 500ms

  disp.setScrollElement(2, 0b10011001); // replace the 3rd element ('y') with a custom pattern

  byte t = disp.getScrollElement(5); // get the 6th element ('t')
  Serial.println(t); // should output 120

  Serial.println(disp.getScrollerLength()); // get the length of the scroll effect: 14
}
void loop() 
{
  // refresh the display in the loop
  disp.refresh();
}