/*
  DonutStudioSevenSegment.h - Library for controlling a seven-segment-display with n digits.
  Created by Donut Studio, March 08, 2023.
  Released into the public domain.
*/

/*
--- seven segment display ---

       D1        D2       D3        D4        

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
  /*
    --- PUBLIC ---
  */
  public:
    /*
      --- CONSTRUCTOR ---
    */

    // constructor of the class with the pins to set (gnd represents the D1, D2, ... pins from the display; the array has to be ordered descending)
    SegmentController(int a, int b, int c, int d, int e, int f, int g, int dp, int gnd[], int length);


    /*
      --- METHODS ---
    */

    /*-- MAIN --*/

    // initialize the display with the pin type, refresh time and brightness
    void initialize(bool commonAnode, byte refreshTime, byte brightness);
    // refreshes the currently displayed byte
    void refresh();
    // clears the currently displayed byte
    void clear();
    // moves the currently displayed bytes to the right(positive shift value) or left(negative shift value) by the shift amount
    void transform(int shift);


    /*-- SETTINGS --*/
    
    // set the brightness of the display 
    void setBrightness(byte brightness);
    // returns the brightness of the display
    byte getBrightness();
    // set the blink interval of the display
    void setBlinkInterval(int blinkInterval);
    // returns the blink interval of the display
    int getBlinkInterval();
    // set the unidentified char displayment byte
    void setUnidentifiedCharByte(byte b);
    // returns the unidentified char displayment byte
    byte getUnidentifiedCharByte();
    // set the update time of the text in milliseconds
    void setTextUpdate(int updateTime);
    // returns the update time of the text
    int getTextUpdate();


    /*-- DISPLAY --*/

    // display an byte array
    void setByte(byte b[]);
    // display an integer
    void setInt(int number, bool showLeadZeros = false);
    // display an float
    void setFloat(float number);
    // display an string
    void setString(String text, int shift = 0);
    // display an string scrolling through the display
    void setText(String text);


    /*-- GET --*/

    // returns the corresponding byte for a number (0-9)
    byte getNumber(int number);
    // returns the corresponding byte for a character
    byte getCharacter(char character);
    // returns the minus byte
    byte getMinus();
    // returns the dot byte
    byte getDot();


    /*-- BYTES --*/

    // returns the byte with the proper segments activated
    byte addByte(byte b, byte addition);
    // returns the byte with the proper segments deactivated
    byte subtractByte(byte b, byte subtraction);
    // returns the inversed byte
    byte inverseByte(byte b);
    // returns the byte with one segment activated/deactivated
    byte setByteSegment(byte b, byte segmentIndex, bool value);
    // returns true if the segment is active and false if not
    bool byteSegmentActive(byte b, byte segmentIndex);

    
    /*-- DISPLAY BYTES --*/

    // sets the byte of the currently displayed digit
    void setDigit(int digitIndex, byte b);
    // gets the byte of the currently displayed digit
    byte getDigit(int digitIndex);
    // sets a segment of the byte from the currently display digit
    void setDigitSegment(int digitIndex, byte segmentIndex, bool value);
    // gets the segment state of the byte from the currently display digit
    bool digitSegmentActive(int digitIndex, byte segmentIndex);
    
    

    /*-- EFFECTS --*/

    // enable/disable one digit
    void setDigitState(int digitIndex, bool value);
    // enable/disable all digits
    void setDigitStateAll(bool value);
    // get the state of a digit
    bool getDigitState(int digitIndex);

    // enable/disable blinking on one digit
    void setBlinking(int digitIndex, bool value);
    // enable/disable blinking on all digits
    void setBlinkingAll(bool value);
    // get the blinking state of a digit
    bool getBlinking(int digitIndex);

    // reset blinking and deactivated digits
    void resetEffects();


    /*-- OTHER --*/

    // check if an string is empty
    bool isStringEmpty(String s);
    // check if an integer is able to be displayed
    bool isNumberInRange(int number);
    // check if an float is able to be displayed
    bool isNumberInRange(float number);
    // check if an digit index is inside of the range
    bool isDigitInRange(int digitIndex);
    // check if an segment index is inside of the range
    bool isSegmentInRange(byte segmentIndex);


    /*
      --- PRIVATE ---
    */
  private:
    /*
      --- METHODS ---
    */

    // set the segments according to a byte
    void setSegments(byte b);
    // update the brightness according to the pin type
    void updateBrightness();
    // shifts one digit into another
    void transformDigit(int digitIndex, int shift);

    // get the length of the segments
    int getSegmentLength();
    // check if the text is set and should be displayed
    bool hasText();

    // updated the display text to create a scroll effect
    void updateDisplayText();
    // cleares the text which is currently displayed
    void clearDisplayText();
    
    
    /*
      --- VARIABLES ---
    */

    // the byte which is currently displayed
    byte _currentDisplayByte[MAXDIGITS];

    // brightness of the display max=255
    byte _brightness = 200;
    // time (in milliseconds) of blinking digits
    int _blinkInterval = 250;
    // the byte displayed if the user uses an unidentified char
    byte _unidentifiedChar = B01011101;

    // the text which should be displayed
    String _displayText;
    // the length of the text
    int _displayTextLength = 0;
    // the last timestamp when the text was transformed
    unsigned long _lastTextChange;
    // the index of the display text
    int _displayTextIndex = 0;
    // the speed of the text (the smaller, the faster it is)
    int _textUpdateTime = 250;
    
    // array of the digits that blink
    bool _blinkDigit[MAXDIGITS];
    // array of the digits that are disabled
    bool _enabledDigit[MAXDIGITS];
    


    /*-- CONST --*/

    // all digits from 0-9 and off
    byte _digits[11] = { B00111111, B00000110, B01011011, B01001111, B01100110, B01101101, B01111101, B00000111, B01111111, B01101111, B00000000 };
    // every singe segment beginning from a and going to the decimal point
    byte _segments[8] = { B00000001, B00000010, B00000100, B00001000, B00010000, B00100000, B01000000, B10000000 };
    // the alphabet: a b c d e f g h i j k l m n o p q r s t u v w x y z
    byte _alphabet[26] = { B01110111, B01111100, B01011000, B01011110, B01111001, B01110001, B00111101, B01110100, B00000100, B00011110, B01110101, B00111000, B01010101, B01010100, B01011100, B01110011, B01100111, B01010000, B00101101, B01111000, B00011100, B00101010, B01101010, B01110110, B01101110, B00011011 };
    // special characters to display: ! ? [( )] / \ *° " ' + _ : ; = > < ^ , 
    byte _specialCharacters[18] = { B10000010, B01010011, B00111001, B00001111, B01010010, B01100100, B01100011, B00100010, B00100000, B01110000, B00001000, B01001000, B01001100, B01000001, B01000011, B01100001, B00100011, B00001100 };

    // type of the display: common cathode-0, command anode-1
    int _commonPinType = 0;
    // time (in milliseconds) to refresh the display
    byte _refreshTime = 2;
    // is a pin for the decimal point given?
    bool _hasDP = true;

    // pins for the segments: a, b, c, d, e, f, g, dp
    int _segmentPins[8];
    // ground pins for the digits
    int _gndPins[MAXDIGITS];
    // the length of the display
    int _displayLength = MAXDIGITS;
};
#endif