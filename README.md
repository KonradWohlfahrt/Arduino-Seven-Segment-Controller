# SEVEN SEGMENT DISPLAY CONTROLLER LIBRARY README
Welcome!
This is my solution for controlling a seven segment display with n digits (maximum=8).
The library is able to display an int, float, string or your own symbol. You can also enable/disable digits and let them blink.
Feel free to use it anywere in your projects.
Cheers, Donut Studio!


***
# Features
- control a seven segment display directly with you ATmega without having to use another controller
- display integers, floats, strings and your own symbols
- enable/disable digits
- enable/disable blinking on digits
- shift the displayed bytes to the right and left


***
# TODO
- redo the code for displaying a float (can be bypassed by converting the float into a string and displaying it)


***
# Installation
1. download the `DonutStudioSevenSegment` folder and extract it into the libraries folder of the Arduino Software
2. start your arduino ide and open the example sketch at `File/Examples/DonutStudioSevenSegment/...`
3. connect the pins of the display with your controller
4. upload the sketch


***
# Usage
To use the library you have to include it in your sketch:
```cpp
// include the libraray
#include "DonutStudioSevenSegment.h"
```

Then you have to define your ground pins and create an object of the class with the following settings: segment pins (A-G), dp, groundPins, display length
```cpp
// define the ground pins
int gnd[] = { 10, 11, 12, 13 };
// create an object of the SegmentController class and define the segment, ground pins and display length.
SegmentController disp = SegmentController(2, 3, 4, 5, 6, 7, 8, 9, gnd, 4);
```

In your `setup()` method you can now initialize the display with a few parameters:
```cpp
// pin type (is the display common anode?), refresh time (if you see flickering reduce this value), brightness(0-255)
disp.initialize(true, 2, 200);
```

To display something on the display you firstly have to call one of those methods at least once:
```cpp
// display your own symbol represented as a byte array with the length of your display length
disp.setByte(byte d[])
// number, should the display show lead zeros?  -   default: false
disp.setInt(123, true)
// number
disp.setFloat(12.54f)
// string
disp.setString("hey!");
// text which scrolls through the display
disp.setText("hey, i'm a cool person!");
```
Finally, you have to call the `disp.refresh();` method in your loop.


***
# BYTE DISPLAYMENT
The library supports to display your own symbols.
A byte is used to enable or disable the segments.
`B01110111` would be the letter `A` with common cathode.
Each bit represents one segment of the digit.
The first bit starting from right represents the A-Segment, going to the seventh bit representing the G-Segment. The final bit represents the decimal pin.
If you are using a common cathode, one means on and zero means off.
At a common anode it's inversed.

There are multiple methods to manipulate a byte for the display (more can be found in the docs below):
- `byte addByte(byte b, byte addition);` Activates some segments of a byte: b = B01110101, addition= B10000010 => B11110111 (B01110101 + B10000010)
- `byte setByteSegment(byte b, byte segmentIndex, bool value);` Activates/Deactivates a given segment (0=A - 6=G, 7=dp): b = B01110111, segmentIndex = 7, value = true => B11110111


***
# Documentation
CONSTRUCTOR
- `SegmentController(int a, int b, int c, int d, int e, int f, int g, int dp, int gnd[], int length);` => constructor of the class with the pins to set (gnd represents the D1, D2, ... pins from the display; the array has to be ordered descending)    

MAIN
- `void initialize(bool commonAnode, byte refreshTime, byte brightness);` => initialize the display with the pin type, refresh time and brightness
- `void refresh();` => refreshes the currently displayed byte
- `void clear();` => clears the currently displayed byte
- `void transform(int shift);` => moves the currently displayed bytes to the right(positive shift value) or left(negative shift value) by the shift amount

SETTINGS
- `void setBrightness(byte brightness);` => set the brightness of the display 
- `byte getBrightness();` => returns the brightness of the display
- `void setBlinkInterval(int blinkInterval);` => set the blink interval of the display
- `int getBlinkInterval();` => returns the blink interval of the display
- `void setUnidentifiedCharByte(byte b);` => set the unidentified char displayment byte
- `byte getUnidentifiedCharByte();` => // returns the unidentified char displayment byte
- `void setTextUpdate(int updateTime);` => set the update time of the text in milliseconds
- `int getTextUpdate();` => returns the update time of the text

DISPLAY
- `void setByte(byte b[]);` => display an byte array
- `void setInt(int number, bool showLeadZeros = false);` => display an integer
- `void setFloat(float number);` => display an float
- `void setString(String text, int shift = 0);` => display an string
- `void setText(String text);` => display an string scrolling through the display

GET
- `byte getNumber(int number);` => returns the corresponding byte for a number (0-9)
- `byte getCharacter(char character);` => returns the corresponding byte for a character
- `byte getMinus();` => returns the minus byte
- `byte getDot();` => returns the dot byte

BYTES
- `byte addByte(byte b, byte addition);` => returns the byte with the proper segments activated
- `byte subtractByte(byte b, byte subtraction);` => returns the byte with the proper segments deactivated
- `byte inverseByte(byte b);` => returns the inversed byte
- `byte setByteSegment(byte b, byte segmentIndex, bool value);` => returns the byte with one segment activated/deactivated
- `bool byteSegmentActive(byte b, byte segmentIndex);` => returns true if the segment is active and false if not

DISPLAY BYTES
- `void setDigit(int digitIndex, byte b);` => sets the byte of the currently displayed digit
- `byte getDigit(int digitIndex);` => gets the byte of the currently displayed digit
- `void setDigitSegment(int digitIndex, byte segmentIndex, bool value);` => sets a segment of the byte from the currently display digit
- `bool digitSegmentActive(int digitIndex, byte segmentIndex);` => gets the segment state of the byte from the currently display digit

EFFECTS
- `void setDigitState(int digitIndex, bool value);` => enable/disable one digit
- `void setDigitStateAll(bool value);` => enable/disable all digits
- `bool getDigitState(int digitIndex);` => get the state of a digit
- `void setBlinking(int digitIndex, bool value);` => enable/disable blinking on one digit
- `void setBlinkingAll(bool value);` => enable/disable blinking on all digits
- `bool getBlinking(int digitIndex);` => get the blinking state of a digit
- `void resetEffects();` => reset blinking and deactivated digits

OTHER
- bool isStringEmpty(String s);`` => check if an string is empty
- `bool isNumberInRange(int number);` => check if an integer is able to be displayed
- `bool isNumberInRange(float number);` => // check if an float is able to be displayed
- `bool isDigitInRange(int digitIndex);` => check if an digit index is inside of the range
- `bool isSegmentInRange(byte segmentIndex);` => check if an segment index is inside of the range


***
# Credits
DonutStudioSevenSegment.h - Library for controlling a seven-segment-display with n digits.
Created by Donut Studio, March 08, 2023.
Released into the public domain.

![example](https://github.com/Donut-Studio/Arduino-Seven-Segment-Controller/blob/main/assets/int.gif)
![example](https://github.com/Donut-Studio/Arduino-Seven-Segment-Controller/blob/main/assets/float.gif)
![example](https://github.com/Donut-Studio/Arduino-Seven-Segment-Controller/blob/main/assets/byte.gif)
![example](https://github.com/Donut-Studio/Arduino-Seven-Segment-Controller/blob/main/assets/string.gif)
![example](https://github.com/Donut-Studio/Arduino-Seven-Segment-Controller/blob/main/assets/text.gif)