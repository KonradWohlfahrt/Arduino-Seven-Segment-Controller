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


// byte (already inverted for common anode)
byte b[4] = { B10101010, B01111001, B10001000, B10000011 };

unsigned long nextUpdate = 0;
int updateTime = 1500;

int state = -1;

void setup() 
{
  // start the serial connection
  Serial.begin(9600);
  // initialize the display (common anode) with a refresh time of 2 and brightness of 175
  disp.initialize(true, 1, 175);

  // send a message
  Serial.println("int displayment");
  // display '0000' on the display
  disp.setInt(0, true);

  // set the update time for the next state
  nextUpdate = millis() + updateTime;
}
void loop() 
{
  // refresh the display
  disp.refreshDisplay();
  
  if (millis() > nextUpdate)
  {
    state = (state + 1) % 20;
    disp.resetEffects();

    switch(state)
    {
      // check blinking
      case 0:
        Serial.println("--blink test--");
        disp.setBlinkInterval(250);
        disp.setInt(0, true);
        disp.setBlink(0, true);
        break;
      case 1:
        disp.setBlinkInterval(125);
        disp.setBlink(1, true);
        break;
      case 2:
        disp.setBlinkInterval(500);
        disp.setBlink(2, true);
        break;
      case 3:
        disp.setBlinkInterval(200);
        disp.setBlink(3, true);
        break;

      // float displayment
      case 4:
        Serial.println("--float test--");
        disp.setFloat(12.34);
        break;

      // check disable
      case 5:
        Serial.println("--disable test--");
        disp.setDigit(0, false);
        break;
      case 6:
        disp.setDigit(1, false);
        break;
      case 7:
        disp.setDigit(2, false);
        break;
      case 8:
        disp.setDigit(3, false);
        break;

      // byte displayment
      case 9:
        Serial.println("--byte test--");
        disp.setByte(b);
        break;

      // check byte manipulation
      case 10:
        Serial.println("--byte manipulation test--");
        Serial.println("replace");

        b[0] = disp.getMinus();
        b[1] = disp.getDot();
        b[2] = disp.getDigit(5);
        b[3] = disp.getCharacter('*');

        disp.setByte(b);
        break;
      case 11:
        Serial.println("add, subtract, segment, invert");

        b[0] = disp.addByte(b[0], B10110110);
        b[1] = disp.subtractByte(b[1], B01111111);
        b[2] = disp.setSegment(b[2], 7, true);
        b[3] = disp.inverseByte(b[3]);

        disp.setByte(b);
        break;
      case 12:
        Serial.println("display byte segment");

        disp.setDisplaySegment(0, 6, true);
        disp.setDisplaySegment(1, 2, true);
        disp.setDisplaySegment(2, 6, false);
        disp.setDisplaySegment(3, 3, false);
        break;

      // string displayment
      case 13:
        Serial.println("--string test--");
        disp.setString("Hey!");
        break;

      // check string transform
      case 14:
        Serial.println("string transform");
        disp.setString("Hey!", 1);
        break;
      case 15:
        disp.setString("Hey!", -1);
        break;

      // check unknown character
      case 16:
        Serial.println("unknown char");
        disp.setString("|~}%");
        break;

      // text displayment
      case 17:
        Serial.println("--text test--");
        disp.setTextSpeed(200);
        disp.setText("Hey, how are you?");
        break;

      // check text speed
      case 18:
        Serial.println("text speed test");
        disp.setTextSpeed(300);
        break;
      case 19:
        disp.setTextSpeed(100);
        break;
    }

    nextUpdate = millis() + updateTime;
  }
}