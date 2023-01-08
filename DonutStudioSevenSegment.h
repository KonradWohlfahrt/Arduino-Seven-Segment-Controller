/*
  DonutStudioSevenSegment.h - Library for controlling a seven-segment-display with n digits.
  Created by Donut Studio, January 08, 2023.
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
  /*
    --- PUBLIC ---
  */
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
    // refresh the currently displayed byte
    void refreshDisplay();
    // clears the currently displayed byte
    void clearDisplay();


    /*
      settings
    */
    // set the brightness of the display 
    void setBrightness(byte _brightness);
    // set the blink interval of the display
    void setBlinkInterval(unsigned int _blinkInterval);
    // set the unknown char displayment byte
    void setUnknownChar(byte _byte);
    // set the speed of the text: a lower value means faster transforming
    void setTextSpeed(unsigned int _speed);


    /*
      displayment
    */
    // set an byte value to display
    void setByte(byte _digits[]);
    // set an integer value to display
    void setInt(int _number, bool _showLeadZeros = false);
    // set an float value to display
    void setFloat(float _number);
    // sets a string to display
    void setString(String _string, int _transform = 0);
    // sets a string to display with a scrolling effect
    void setText(String _text, bool restart = true);


    /*
      byte manipulation
    */
    // returns the corresponding digit for a number (0-9)
    byte getDigit(int _digit);
    // returns the corresponding byte for a character
    byte getCharacter(char _character);
    // returns the minus digit
    byte getMinus();
    // returns the dot digit
    byte getDot();

    // returns the byte with the proper segments activated
    byte addByte(byte _byte, byte _addition);
    // returns the byte with the proper segments deactivated
    byte subtractByte(byte _byte, byte _subtraction);
    // returns the byte with one segment active/deactive
    byte setSegment(byte _byte, int _segment, bool _value);

    // returns the inversed byte
    byte inverseByte(byte _byte);

    // activates or deactivates one segment of one current displayed digit
    void setDisplaySegment(int _digit, int _segment, bool _value);


    /*
      effects
    */
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


    /*
      other
    */
    // check if the string is empty
    bool isStringEmpty(String _string);
    // check if an integer number is able to be displayed
    bool numberInRange(int _number);
    // check if an float number is able to be displayed
    bool numberInRange(float _number);


    /*
      --- PRIVATE ---
    */
  private:
    /*
      --- METHODS ---
    */
    // set the segments according to a byte
    void setSegments(byte _digit);
    // update the brightness according to the pin type
    void updateBrightness();
    // updated the display text to create a scroll effect
    void updateDisplayText();
    // cleares the text which is currently displayed
    void clearDisplayText();
    
    // get the length of the segments
    int getSegmentLength();
    // check if the text is set and should be displayed
    bool hasText();
    
    
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
    // is a pin for the decimal point given?
    bool hasDP = true;

    // pins for the segments: a, b, c, d, e, f, g, dp
    int segmentPins[8];
    // ground pins for the digits
    int gndPins[MAXDIGITS];
    // the length of the display
    int displayLength = MAXDIGITS;
    // the byte which is currently displayed
    byte currentDisplayByte[MAXDIGITS];

    
    // the translation of the text
    int textTransform = 0;
    // the speed of the text (the smaller the faster it is)
    unsigned int textSpeed = 250;
    // the interval when the text needs to swap
    unsigned long textChangeInterval = 0;
    // the text which should be displayed
    String displayText;
    // the length of the text
    int displayTextLength = 0;
    
    // all digits from 0-9 and off
    byte digits[11] = { B00111111, B00000110, B01011011, B01001111, B01100110, B01101101, B01111101, B00000111, B01111111, B01101111, B00000000 };
    // every singe segment beginning from a and going to the decimal point
    byte segments[8] = { B00000001, B00000010, B00000100, B00001000, B00010000, B00100000, B01000000, B10000000 };
    // the alphabet: a b c d e f g h i j k l m n o p q r s t u v w x y z
    byte alphabet[26] = { B01110111, B01111100, B01011000, B01011110, B01111001, B01110001, B00111101, B01110100, B00000100, B00011110, B01110101, B00111000, B01010101, B01010100, B01011100, B01110011, B01100111, B01010000, B00101101, B01111000, B00011100, B00101010, B01101010, B01110110, B01101110, B00011011 };
    // special characters to display: ! ? [( )] / \ *° " ' + _ : ; = > < ^ , 
    byte specialCharacters[18] = { B10000010, B01010011, B00111001, B00001111, B01010010, B01100100, B01100011, B00100010, B00100000, B01110000, B00001000, B01001000, B01001100, B01000001, B01000011, B01100001, B00100011, B00001100 };
    // the byte display if the user uses an unknown char
    byte unknownChar = B01000001;

    // array of the digits that blink
    bool blinkDigit[MAXDIGITS];
    // array of the digits that are disabled
    bool enabledDigit[MAXDIGITS];
};
#endif