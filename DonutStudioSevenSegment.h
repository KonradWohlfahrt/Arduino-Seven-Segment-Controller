/*
  DonutStudioSevenSegment.h - Library for controlling a seven-segment-display with n digits.
  Created by Donut Studio, December 24, 2022.
  Released into the public domain.
*/

/*
--- seven segment display ---

       D4        D3       D2        D1        

        A
       ---
    |       |
  F |       | B
    |       |
       -G-
    |       |
  E |       | C
    |       |
       ---
        D   - 
            dp
*/



#ifndef DonutStudioSevenSegment.h
#define DonutStudioSevenSegment.h


#ifndef MAXDIGITS
#define MAXDIGITS 8
#endif


#include "Arduino.h"

class SegmentController
{
  public:
    /*
      --- CONSTRUCTOR ---
    */

    // constructor of the class in which you set all the pins for the display
    SegmentController(int a, int b, int c, int d, int e, int f, int g, int dp, int gnd[], int length);

    /*
      --- METHODS ---
    */

    // initialize the display with the pin type, refresh time and brightness
    void initialize(bool _commonAnode, unsigned long _refreshTime = 2, byte _brightness = 175);

    // set the brightness of the display 
    void setBrightness(byte _brightness);
    // set the blink interval of the display
    void setBlinkInterval(int _blinkInterval);

    // refresh the currently displayed byte
    void refreshDisplay();

    // set an byte value to display
    void setByte(byte _digits[]);
    // set an integer value to display
    void setInt(int _number, bool _showLeadZeros = false);
    // set an float value to display
    void setFloat(float _number);

    // returns the corresponding digit to a number (0-9)
    byte getDigit(int _digit);
    // returns the minus digit
    byte getMinus();
    // returns the dot digit
    byte getDot();

    // returns the byte with the segments activated
    byte activateByte(byte _byte, byte _activation);
    // returns the byte with the segments deactivated
    byte deactivateByte(byte _byte, byte _deactivation);

    // returns the byte with one segment activated
    byte addSegment(byte _byte, int _segment);
    // returns the byte with one segment deactivated
    byte removeSegment(byte _byte, int _segment);

    // returns the inversed byte
    byte inverseByte(byte _byte);

    // enable/disable blinking on one digit
    void setBlink(int _digit, bool _value = true);
    // enable/disable blinking on all digits
    void setBlinkAll(bool _value = true);
    // enable/disable one digit
    void setDigit(int _digit, bool _value = false);
    // enable/disable all digits
    void setDigitAll(bool _value = true);
    // reset blinking and deactivated digits
    void resetEffects();
  private:
    /*
      --- METHODS ---
    */

    // set the segments according to a byte
    void setSegments(byte _digit);
    // update the brightness according to the pin type
    void updateBrightness();
    
    // get the length of the segments
    int getSegmentLength();

    // check if an integer number is able to be display
    bool NumberInRange(int _number);
    // check if an float number is able to be display
    bool NumberInRange(float _number);
    
    /*
      --- VARIABLES ---
    */

    // type of the display: common cathode-0, command anode-1
    int commonPinType = 0;
    // time (in milliseconds) to refresh the display
    unsigned long refreshTime = 2;
    // brightness of the display max=255
    byte brightness = 200;
    // time (in milliseconds) of blinking digits
    int blinkInterval = 250;

    // pins for the segments: a, b, c, d, e, f, g, dp
    int segmentPins[8];
    // ground pins for the digits
    int gndPins[MAXDIGITS];
    // the byte which is currently displayed
    byte currentDisplayByte[MAXDIGITS];
    // is a pin for the decimal point given?
    bool hasDP = true;
    // the length of the display
    int displayLength = MAXDIGITS;
    

    // all digits from 0-9 and off
    byte digits[11] = { B00111111, B00000110, B01011011, B01001111, B01100110, B01101101, B01111101, B00000111, B01111111, B01101111, B00000000 };
    // the segments
    byte segments[8] = { B00000001, B00000010, B00000100, B00001000, B00010000, B00100000, B01000000, B10000000 };

    // array of the digits that blink
    bool blinkDigit[MAXDIGITS];
    // array of the digits that are disabled
    bool enabledDigit[MAXDIGITS];
};
#endif