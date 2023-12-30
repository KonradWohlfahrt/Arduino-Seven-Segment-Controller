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



#ifndef DonutStudioSevenSegment.h
#define DonutStudioSevenSegment.h
#endif


#ifndef MAXDIGITS
#define MAXDIGITS 6
#endif

#ifndef MAXSCROLLERSIZE
#define MAXSCROLLERSIZE 64


#include "Arduino.h"

class SegmentController
{
  public:
    SegmentController(bool commonAnode, int segmentPins[8], int digitPins[], byte displayLength, byte refreshTime);

    // refresh the display
    void refresh();
    // clear the display
    void clear();
    // move the display to the right (positive), or left (negative)
    void transform(int shift);
    // set the brightness
    void setBrightness(byte brightness);
    // get the brightness
    byte getBrightness();
    

    // display custom symbols/bytes
    void setByte(byte b[]);
    // display a integer
    void setInt(int number, bool showLeadZeros = false);

    // display a float
    //void setFloat(float number);

    // display a string
    void setString(String text, int shift = 0);

    // change a digit to a new byte
    void setDigit(byte digitIndex, byte b);
    // get the current byte of a digit
    byte getDigit(byte digitIndex);
    // enable/disable a segment of a digit
    void setDigitSegment(byte digitIndex, byte segmentIndex, bool value);
    // check if a segment on a digit is enabled
    bool digitSegmentActive(byte digitIndex, byte segmentIndex);


    // start a scoller with a text
    void setScroller(String text);
    // start a scroller with custom symbols
    void setScroller(byte bytes[], int size);
    // expand the the scroller with a text
    void appendScroller(String text);
    // expand the the scroller with a text
    void appendScroller(byte bytes[], int size);

    // change an element of the scroller
    void setScrollElement(byte index, byte b);
    // get an element of the scroller
    byte getScrollElement(byte index);
    // get the length of the scroller
    byte getScrollerLength();
    // set the time it takes to move to the next character
    void setScrollerUpdateTime(unsigned int updateTime);
    // get the time it takes to move to the next character
    unsigned int getScrollerUpdateTime();


    byte getNumber(int number);
    byte getCharacter(char character);
    byte getMinus();
    byte getDot();
    byte getSegment(byte segmentIndex);


    byte combineBytes(byte byte1, byte byte2);
    byte subtractBytes(byte byte1, byte subtraction);
    // enable/disable a segment of a byte
    byte setSegment(byte byte1, byte segmentIndex, bool value);
    // check if a segment of a byte is enabled/disabled
    bool isSegmentActive(byte byte1, byte segmentIndex);


    // enable/disable a digit
    void setDigitState(byte digitIndex, bool value);
    // enable/disable all digits
    void setDigitStateAll(bool value);
    // check if a digit is enabled/disabled
    bool getDigitState(byte digitIndex);

    // enable/disable blinking on a digit
    void setBlinking(byte digitIndex, bool value);
    // enable/disable blinking on all digits
    void setBlinkingAll(bool value);
    // check if a digit is blinking
    bool getBlinking(byte digitIndex);

    // set the interval for blinking
    void setBlinkInterval(unsigned int blinkInterval);
    // get the interval for blinking
    unsigned int getBlinkInterval();

    // reset all effects
    void resetEffects();


    // check if a number can be shown on the display
    bool isNumberInRange(int number);
    // check if a number can be shown on the display
    bool isNumberInRange(float number);
    bool isDigitInRange(byte digitIndex);
    bool isSegmentInRange(byte segmentIndex);



  private:
    void setSegments(byte b);
    void transformDigit(byte digitIndex, int shift);

    int getSegmentLength();
    bool isCommonAnode();
    bool isStringEmpty(String s);

    void updateScroller();
    void disableScroller();
    
    

    // display type: common cathode = 0, command anode = 1
    int _commonPinType = 0;
    // a, b, c, d, e, f, g, dp
    int _segmentPins[8];
    // digit pins in ascending order (D1, D2, ...)
    int _digitPins[MAXDIGITS];
    // amount of digits
    byte _displayLength = MAXDIGITS;

    // time (in milliseconds) to refresh the display
    byte _refreshTime = 2;
    // time (in milliseconds) to blinking a digits
    unsigned int _blinkInterval = 250;
    byte _brightness = 255;
    bool _hasDP = true;

    // bytes currently displayed on the device
    byte _displayedBytes[MAXDIGITS];
    

    bool _digitBlinking[MAXDIGITS];
    bool _digitEnabled[MAXDIGITS];


    bool _isScrolling = false;
    byte _scroller[MAXSCROLLERSIZE];
    byte _scrollerLength = 0;
    byte _currentScrollIndex = 0;
    unsigned int _scrollUpdateTime = 300;
    unsigned long _previousScrollTime;


    // all digits from 0-9 and off
    const byte _digits[11] = { 0b00111111, 0b00000110, 0b01011011, 0b01001111, 0b01100110, 0b01101101, 0b01111101, 0b00000111, 0b01111111, 0b01101111, 0b00000000 };
    // the alphabet
    const byte _alphabet[26] = { 0b01110111, 0b01111100, 0b01011000, 0b01011110, 0b01111001, 0b01110001, 0b00111101, 0b01110100, 0b00000100, 0b00011110, 0b01110101, 0b00111000, 0b01010101, 0b01010100, 0b01011100, 0b01110011, 0b01100111, 0b01010000, 0b00101101, 0b01111000, 0b00011100, 0b00101010, 0b01101010, 0b01110110, 0b01101110, 0b00011011 };
    // some special characters: , ! ? = > < ( ) / \ * " ^  
    const byte _specialCharacters[13] = { 0b00001100, 0b10000010, 0b01010011, 0b01001000, 0b01001100, 0b01011000, 0b00111001, 0b00001111, 0b01010010, 0b01100100, 0b01100011, 0b00100010, 0b00100011 };
};
#endif