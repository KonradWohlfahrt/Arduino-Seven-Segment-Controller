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
- enable/disable digits at runtime
- enable/disable blinking on digits at runtime


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
// text which scrolls through the screen
disp.setText("hey, i'm a cool person!");
```
Finally, you have to call the `disp.refreshDisplay();` method in your loop.


***
# BYTE DISPLAYMENT
The library supports to display your own symbols.
A byte is used to enable or disable the segments.
`B01110111` would be the letter `A` with common cathode.
Each bit represents one segment of the digit.
The first bit starting from right represents the A-Segment, going to the seventh bit representing the G-Segment. The final bit represents the decimal pin.
If you are using a common cathode, one means on and zero means off.
At a common anode it's inversed.

There are multiple methods to manipulate a display-byte:
- `byte activateByte(byte _byte, byte _activation);` Activates some segments of a byte: B01110111 + B10000000 = B11110111
- `byte addSegment(byte _byte, int _segment);` Activates a given segment (0=A - 6=G, 7=dp): B01110111, 7 = B11110111
- `byte inverseByte(byte _byte);` Inverses a byte: B01110111 = B10001000


***
# Documentation
CONSTRUCTOR
- `SegmentController(int a, int b, int c, int d, int e, int f, int g, int dp, int gnd[], int length);` => constructor of the class in which you set all the pins and length for the display

SET UP
- `void initialize(bool _commonAnode, int _refreshTime = 2, byte _brightness = 175);` => initialize the display with the pin type, refresh time and brightness
- `void refreshDisplay();` => refresh the display
- `void clearDisplay();` => clears the currently displayed byte

SETTINGS
- `void setBrightness(byte _brightness);` => set the brightness of the display 
- `void setBlinkInterval(unsigned int _blinkInterval);` => set the blink interval of the display
- `void setUnknownChar(byte _byte);` => set the unknown char displayment byte
- `void setTextSpeed(unsigned int _speed);` => set the speed of the text: a lower value means faster transforming

DISPLAY
- `void setByte(byte d[]);` => set an byte value to display
- `void setInt(int number, bool showLeadZeros = false);` => set an integer value to display
- `void setFloat(float number);` => set an float value to display (NOTE: not completely working)
- `void setString(String _string, int _transform = 0);` => sets a string to display
- `void setText(String _text, bool restart = true);` => sets a string to display with a scrolling effect

MANIPULATING BYTES
- `byte getDigit(int _digit);` => returns the corresponding digit for a number (0-9)
- `byte getCharacter(char _character);` => returns the corresponding byte for a character
- `byte getMinus();` => returns the minus digit
- `byte getDot();` => returns the dot digit
- `byte addByte(byte _byte, byte _addition);` => returns the byte with the segments activated
- `byte subtractByte(byte _byte, byte _subtraction);` => returns the byte with the segments deactivated
- `byte setSegment(byte _byte, int _segment, bool _value);` => returns the byte with one segment active/deactive
- `byte inverseByte(byte _byte);` => returns the inversed byte
- `void setDisplaySegment(int _digit, int _segment, bool _value);` => activates or deactivates one segment of one current displayed digit

EFFECTS
- `void setBlink(int _digit, bool _value);` => enable/disable blinking on one digit
- `void setBlinkAll(bool _value);` => enable/disable blinking on all digits
- `void setDigit(int _digit, bool _value);` => enable/disable one digit
- `void setDigitAll(bool _value);` => enable/disable all digits
- `void resetEffects();` => reset blinking and deactivated digits

OTHER
- `bool isStringEmpty(String _string);` => check if the string is empty
- `bool numberInRange(int _number);` => check if an int number is able to be displayed
- `bool numberInRange(float _number);` => check if an float number is able to be displayed

***
# Credits
DonutStudioSevenSegment.h - Library for controlling a seven-segment-display with n digits.
Created by Donut Studio, January 08, 2023.
Released into the public domain.

![example](https://github.com/Donut-Studio/Arduino-Seven-Segment-Controller/blob/main/assets/int.gif)
![example](https://github.com/Donut-Studio/Arduino-Seven-Segment-Controller/blob/main/assets/float.gif)
![example](https://github.com/Donut-Studio/Arduino-Seven-Segment-Controller/blob/main/assets/byte.gif)
![example](https://github.com/Donut-Studio/Arduino-Seven-Segment-Controller/blob/main/assets/string.gif)
![example](https://github.com/Donut-Studio/Arduino-Seven-Segment-Controller/blob/main/assets/text.gif)