# SEVEN SEGMENT DISPLAY CONTROLLER LIBRARY README
Welcome!
This is my solution for controlling a seven segment display with n digits (maximum=8).
Feel free to use it anywere in your projects.
Cheers, Donut Studio!


***
# Features
- control a seven segment display directly with you ATmega without having to use another controller
- display integers, floats and your own symbols
- enable/disable digits at runtime
- enable/disable blinking on digits at runtime


***
# TODO
- display negative numbers
- display char[] and strings
- split up blinking to digits and decimal point


***
# Installation
1. download the `DonutStudioSevenSegment` folder and extract it into the libraries folder of the Arduino Software
2. start your arduino ide and open the example sketch at `File/Examples/DonutStudioSevenSegment/DonutStudioSevenSegmentExample`
3. connect the pins of the display with your controller
4. upload the sketch


***
# Usage
At the top of your sketch add those lines:
```cpp
// include the libraray
#include "DonutStudioSevenSegment.h"
// define the ground pins
int gnd[] = { 10, 11, 12, 13 };
// create an object of the SegmentController class and define the segment, ground pins and display length.
SegmentController disp = SegmentController(2, 3, 4, 5, 6, 7, 8, 9, gnd, 4);
```

In your `setup()` method you can now initialize the display with a few parameters;
```cpp
// pin type (is common anode?), refresh time, brightness
disp.initialize(true, 2, 200);
```

To display something on the display you firstly have to call one of those methods at least once:
```cpp
// display your own symbol represented as a byte array with the length of your display length
disp.setByte(byte d[4])
// number, should the display show lead zeros?  -   default: false
disp.setInt(123, true)
// number
disp.setFloat(12.54f)
```
Then you have to call the `refreshDisplay()` method in your loop.

The final file should look something like this:
```cpp
#include "DonutStudioSevenSegment.h"
int gnd[] = { 10, 11, 12, 13 };
SegmentController disp = SegmentController(2, 3, 4, 5, 6, 7, 8, 9, gnd, 4);

void setup() 
{
  disp.initialize(true, 2, 175);

  disp.setInt(1234);
  disp.setDigit(0, false);
  disp.setBlink(1);
}
void loop() 
{
  disp.refreshDisplay();
}
```


***
# Documentation
- `SegmentController(int a, int b, int c, int d, int e, int f, int g, int dp, int gnd[], int length);` => constructor of the class in which you set all the pins and length for the display
---
- `void initialize(bool _commonAnode, int _refreshTime = 2, byte _brightness = 175);` => initialize the display with the pin type, refresh time and brightness
- `void refreshDisplay();` => refresh the currently displayed byte
---
- `void setByte(byte d[]);` => set an byte value to display
- `void setInt(int number, bool showLeadZeros = false);` => set an integer value to display
- `void setFloat(float number);` => set an float value to display
---
- `byte getDigit(int _digit);` => returns the corresponding digit to a number (0-9)
- `byte getMinus();` => returns the minus digit
- `byte addDot(byte _byte);` => returns th byte with the dot added
- `byte inverseByte(byte _byte);` => returns the inversed byte
---
- `void setBrightness(byte _brightness);` => set the brightness of the display
- `void setBlinkInterval(int _blinkInterval);` => set the blink interval of the display
---
- `void setBlink(int _digit, bool _value);` => enable/disable blinking on one digit
- `void setBlinkAll(bool _value);` => enable/disable blinking on all digits
- `void setDigit(int _digit, bool _value);` => enable/disable one digit
- `void setDigitAll(bool _value);` => enable/disable all digits
- `void resetEffects();` => reset blinking and deactivated digits


***
# Credits
DonutStudioSevenSegment.h - Library for controlling a seven-segment-display with n digits.
Created by Donut Studio, December 21, 2022.
Released into the public domain.


![example](https://github.com/Donut-Studio/Arduino-Seven-Segment-Controller/blob/main/assets/example1.gif)