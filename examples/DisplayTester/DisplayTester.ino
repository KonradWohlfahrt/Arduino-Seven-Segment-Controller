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

unsigned long lastUpdate = 0;
int updateInterval = 2000; // 2s

int state = -1;

void setup() 
{
  Serial.begin(9600);

  Serial.println("--- INTEGER ---");
  disp.setInt(1234);

  lastUpdate = millis();
}
void loop() 
{
  // refresh the display
  disp.refresh();
  
  if (millis() - lastUpdate > updateInterval)
  {
    state = (state + 1) % 22;

    // disable all effects
    disp.resetEffects();

    switch(state)
    {
      case 0:
        Serial.println("--- BLINKING ---");
        disp.setInt(1234);
        disp.setBlinkInterval(250);
        disp.setBlinking(0, true);
        break;
      case 1:
        disp.setBlinkInterval(125);
        disp.setBlinking(1, true);
        break;
      case 2:
        disp.setBlinkInterval(500);
        disp.setBlinking(2, true);
        break;
      case 3:
        disp.setBlinkInterval(200);
        disp.setBlinkingAll(true);
        break;

      // brightness
      case 4:
        Serial.println("--- BRIGHTNESS ---");
        disp.setBrightness(175);
        break;
      case 5:
        disp.setBrightness(75);
        break;
      case 6:
        Serial.println("--- BRIGHTNESS ---");
        disp.setBrightness(25);
        break;


      // disable
      case 7:
        Serial.println("--- DISABLE ---");
        disp.setBrightness(255);
        disp.setDigit(0, false);
        break;
      case 8:
        disp.setDigit(1, false);
        break;
      case 9:
        disp.setDigit(2, false);
        break;
      case 10:
        disp.setDigit(3, false);
        break;

      // custom display
      case 11:
        Serial.println("--- CUSTOM DISPLAY ---");
        disp.setByte(b);
        break;

      case 12:

        b[0] = disp.getNumber(2);
        b[1] = disp.getCharacter('*');
        b[2] = disp.getDot();
        b[3] = disp.getSegment(3);

        disp.setByte(b);
        break;

      // byte manipulation
      case 13:
        Serial.println("--- BYTE MANIPULATION ---");

        b[0] = disp.combineBytes(b[0], 0b10010110);
        b[1] = disp.subtractBytes(b[1], 0b00101001);
        b[2] = disp.setSegment(b[2], 7, true);
        b[3] = ~b[3];

        disp.setByte(b);
        break;
      case 14:
        Serial.println("--- SET DISPLAY SEGMENTS ---");

        disp.setDigitSegment(0, 6, true);
        disp.setDigitSegment(1, 2, true);
        disp.setDigitSegment(2, 6, false);
        disp.setDigitSegment(3, 3, false);
        break;

      // string displayment
      case 15:
        Serial.println("--- STRINGS ---");
        disp.setString("Hey!");
        break;

      // check string transform
      case 16:
        Serial.println("--- STRING TRANSFORM ---");
        disp.setString("Hey!", 1);
        break;
      case 17:
        disp.setString("Hey!", -1);
        break;
      case 18:
        disp.transform(-1);
        break;
      case 19:
        disp.transform(-1);
        break;

      // scroller
      case 20:
        Serial.println("--- SCROLLER ---");
        disp.setScrollerUpdateTime(200);
        disp.setScroller("Hey, this is a text!?.");
        break;

      // scroller speed
      case 21:
        Serial.println("--- SCOLLER SPEED ---");
        disp.setScrollerUpdateTime(300);
        break;
      case 22:
        disp.setScrollerUpdateTime(100);
        break;
    }

    lastUpdate = millis();
  }
}