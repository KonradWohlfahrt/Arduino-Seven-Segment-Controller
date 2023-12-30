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
  /* manipulate the byte array
  Serial.begin(9600);

  // --- preexisting characters ---
  b[0] = disp.getNumber(1);
  //b[0] = disp.getCharacter('a');
  //b[0] = disp.getMinus();
  //b[0] = disp.getDot();
  //b[0] = disp.getSegment(6);
 
  // --- de/activate segments ---
  b[1] = disp.setSegment(b[1], 7, true); // activate the dot
  //b[1] = disp.setSegment(b[1], 1, false); // deactivate the b segment

  // --- combine/subtract ---
  b[2] = disp.combineBytes(b[2], 0b11000000); // activate the dot and g segment
  //b[2] = disp.subtractBytes(b[2], 0b10000100); // deactivate the dot and c segment

  // --- get the segment ---
  Serial.print("A Segment Active on D2: ");
  Serial.println(disp.isSegmentActive(b[1], 0));
  Serial.print("A Segment Active on D4: ");
  Serial.println(disp.isSegmentActive(b[3], 0));

  disp.setByte(b);
  */

  disp.setByte(b); // display the array on the device
  disp.setDigit(0, 0b01010101); // change the first digit
}
void loop() 
{
  // refresh the display in the loop
  disp.refresh();
}