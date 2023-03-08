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

#include "Arduino.h"
#include "DonutStudioSevenSegment.h"

/*
  --- --- CONSTRUCTOR --- ---
*/

SegmentController::SegmentController(int a, int b, int c, int d, int e, int f, int g, int dp, int gnd[], int length)
{
  // set the segment pins
  _segmentPins[0] = a;
  _segmentPins[1] = b;
  _segmentPins[2] = c;
  _segmentPins[3] = d;
  _segmentPins[4] = e;
  _segmentPins[5] = f;
  _segmentPins[6] = g;
  _segmentPins[7] = dp;
  // check if the dp will be used
  _hasDP = dp > 0;
  
  // set the display length
  _displayLength = length;
  if (_displayLength > MAXDIGITS)
    _displayLength = MAXDIGITS;

  // set the ground pins
  for (int i = 0; i < _displayLength; i++)
    _gndPins[i] = gnd[i];
}


/*
  --- --- PUBLIC METHODS --- ---
*/

/*
  --- MAIN ---
*/

void SegmentController::initialize(bool commonAnode, byte refreshTime, byte brightness)
{
  // set the common pin type 
  _commonPinType = commonAnode ? 1 : 0;
  // set the display refresh time
  _refreshTime = refreshTime;
  // set the brightness of the display
  setBrightness(brightness);

  // inverse the bytes if the display is common anode
  if (_commonPinType == 1) 
  {
    // inverse digits
    for (int i = 0; i < sizeof(_digits) / sizeof(_digits[0]); i++) 
      _digits[i] = inverseByte(_digits[i]);
    // inverse segments
    for (int i = 0; i < sizeof(_segments) / sizeof(_segments[0]); i++)
      _segments[i] = inverseByte(_segments[i]);
    // inverse the alphabet
    for (int i = 0; i< sizeof(_alphabet) / sizeof(_alphabet[0]); i ++)
      _alphabet[i] = inverseByte(_alphabet[i]);
    // inverse the special characters
    for (int i = 0; i< sizeof(_specialCharacters) / sizeof(_specialCharacters[0]); i ++)
      _specialCharacters[i] = inverseByte(_specialCharacters[i]);
    // inverse the unknown byte
    _unidentifiedChar = inverseByte(_unidentifiedChar);
  }

  // set the segment pins as output and deactivate them
  for (int i = 0; i < getSegmentLength(); i++)
  {
    pinMode(_segmentPins[i], OUTPUT);
    digitalWrite(_segmentPins[i], 1 - _commonPinType);
  }

  // set the ground pins as output and deactivate them, reset the effects and displayment byte
  for (int i = 0; i < _displayLength; i++)
  {
    _blinkDigit[i] = false;
    _enabledDigit[i] = true;
    _currentDisplayByte[i] = _digits[10];
    pinMode(_gndPins[i], OUTPUT);
    digitalWrite(_gndPins[i], 1 - _commonPinType);
  }
}
void SegmentController::refresh()
{
  // update the brightness if the display uses common anode
  updateBrightness();
  // if a scroll text is set, update the position
  if (hasText())
    updateDisplayText();

  // go through the dispay length (digits)
  for (int i = 0; i < _displayLength; i++)
  {
    // go to next digit if the current one is not enabled
    if (!_enabledDigit[i])
      continue;
    // wait and go to the next digit if the current one should be blinking
    else if (_blinkDigit[i] && (millis() % (_blinkInterval * 2) > _blinkInterval))
    {
      delay(_refreshTime);
      continue;
    }
    
    // activate the digit
    analogWrite(_gndPins[i], (byte)(255 - constrain(_brightness, 0, 255)));
    // set the segment according to the display byte
    setSegments(_currentDisplayByte[i]);
    
    // wait
    delay(_refreshTime);
    
    // deactivate the digit
    analogWrite(_gndPins[i], 255 * (1 - _commonPinType));
    // deactivate all segments
    setSegments(_digits[10]);
  }

  // wait
  delay(_refreshTime);
}
void SegmentController::clear()
{
  // go through the digits and reset them
  for (int i = 0; i < _displayLength; i++)
    _currentDisplayByte[i] = _digits[10];
  // clear the text if it is set
  if (hasText())
    clearDisplayText();
}
void SegmentController::transform(int shift)
{
  // return if nothing will be shifted
  if (shift == 0)
    return;

  // if the shift is positive go from 0 to the display length(exclusive)
  if (shift > 0) 
    for (int i = 0; i < _displayLength; i++)
      transformDigit(i, shift);
  // if the shift is negative go from the display length(exclusive) to 0
  else
    for (int i = _displayLength - 1; i >= 0; i--)
      transformDigit(i, shift);
}


/*
  --- SETTINGS ---
*/

void SegmentController::setBrightness(byte brightness)
{
  // return if the brightness is below 0
  if (brightness < 0)
    return;
  // set the brightness
  _brightness = brightness;
}
byte SegmentController::getBrightness()
{
  // return the current brightness
  return _brightness;
}
void SegmentController::setBlinkInterval(int blinkInterval)
{
  // return if the blink interval is below or equal to 0
  if (blinkInterval <= 0)
    return;
  // set the blink interval
  _blinkInterval = blinkInterval;
}
int SegmentController::getBlinkInterval()
{
  // return the current blink interval
  return _blinkInterval;
}
void SegmentController::setUnidentifiedCharByte(byte b)
{
  // set the unidentified character
  _unidentifiedChar = b;
}
byte SegmentController::getUnidentifiedCharByte()
{
  // set the current unidentified character
  return _unidentifiedChar;
}
void SegmentController::setTextUpdate(int updateTime)
{
  // return if the time is below 0
  if (updateTime < 0)
     return;
  // set the text time
  _textUpdateTime = updateTime;
}
int SegmentController::getTextUpdate()
{
  // return the current text update time
  return _textUpdateTime;
}


/*
  --- DISPLAY ---
*/

void SegmentController::setByte(byte b[])
{
  // clear the text if it is set
  if (hasText())
    clearDisplayText();

  // go through the digits and set the display bytes
  for (int i = 0; i < _displayLength; i++)
    _currentDisplayByte[i] = b[i];
}
void SegmentController::setInt(int number, bool showLeadZeros)
{
  // check if the number is in range to be displayed
  if (!isNumberInRange(number))
    return;
  // clear the text if it is set
  if (hasText())
    clearDisplayText();
  
  bool lead = !showLeadZeros;
  bool negativ = number < 0;
  if (negativ)
    number *= -1;

  for (int i = _displayLength - 1; i >= 0 ; i--)
  {
    if (negativ && !lead && i == _displayLength - 1)
    {
      _currentDisplayByte[i] = getMinus();
      negativ = false;
      continue;
    }

    byte digit = (int)(number / pow(10, i)) % 10;
    if (negativ && digit == 0 && i > 0)
    {
      byte d = (int)(number / pow(10, i - 1)) % 10;
      if (d != 0)
      {
        _currentDisplayByte[i] = getMinus();
        negativ = false;
      }
    }
    else if (lead && digit == 0)
      _currentDisplayByte[i] = _digits[10];
    else
    {
      _currentDisplayByte[i] = _digits[digit];
      lead = false;
    }
  }
}
void SegmentController::setFloat(float number)
{
  // check if the number is in range to be displayed
  if (!isNumberInRange(number))
    return;
  // clear the text if it is set
  if (hasText())
    clearDisplayText();

  bool negativ = number < 0;
  if (negativ)
    number *= -1;

  int exponent = 0;
  for (int i = -_displayLength + 2; i <= _displayLength; i++)
  {
    if (number < pow(10, i))
    {
      exponent = i - 1;
      break;
    }
  }
  
  int dotIndex = _displayLength - 1;
  if(exponent > 0)
    dotIndex -= exponent;

  number = number * pow(10, dotIndex);
  for (int i = 0; i < _displayLength; i++)
  {
    byte digit = (int)(number / pow(10, i)) % 10;

    if (i == dotIndex)
      _currentDisplayByte[i] = setByteSegment(_digits[digit], 7, true);
    else
      _currentDisplayByte[i] = _digits[digit];
  }
}
void SegmentController::setString(String text, int shift) 
{
  // check if the string to be displayed is empty
  if (isStringEmpty(text))
    return;
  
  ///* TODO probably not needed


  // clear the text if 
  if (hasText() && !_displayText.equals(text))
    clearDisplayText();

  //*/

  for (int i = _displayLength - 1; i >= 0 ; i--)
  {
    int index = (_displayLength - 1) - i;
    index += shift;

    if (index < 0 || index >= text.length())
      _currentDisplayByte[i] = _digits[10];
    else 
      _currentDisplayByte[i] = getCharacter(text[index]);
  }
}
void SegmentController::setText(String text) 
{
  // check if the string to be displayed is empty
  if (isStringEmpty(text))
    return;

  // set the text and its length
  _displayText = text;
  _displayTextLength = _displayText.length();

  // set the display text index to -1
  _displayTextIndex = -1;

  // set the last timestamp to the current milliseconds
  _lastTextChange = millis();
}


/*
  --- GET ---
*/

byte SegmentController::getNumber(int number)
{
  // checks if the number is between 0-9
  if (number < 0 || number > 9)
    return _digits[10];
  // returns the byte representing the number
  return _digits[number];
}
byte SegmentController::getCharacter(char character)
{
  // select a proper character from the following characters:

  character = tolower(character);
  switch (character)
  {
    // alphabet
    case 'a':
      return _alphabet[0];
    case 'b':
      return _alphabet[1];
    case 'c':
      return _alphabet[2];
    case 'd':
      return _alphabet[3];
    case 'e':
      return _alphabet[4];
    case 'f':
      return _alphabet[5];
    case 'g':
      return _alphabet[6];
    case 'h':
      return _alphabet[7];
    case 'i':
      return _alphabet[8];
    case 'j':
      return _alphabet[9];
    case 'k':
      return _alphabet[10];
    case 'l':
      return _alphabet[11];
    case 'm':
      return _alphabet[12];
    case 'n':
      return _alphabet[13];
    case 'o':
      return _alphabet[14];
    case 'p':
      return _alphabet[15];
    case 'q':
      return _alphabet[16];
    case 'r':
      return _alphabet[17];
    case 's':
      return _alphabet[18];
    case 't':
      return _alphabet[19];
    case 'u':
      return _alphabet[20];
    case 'v':
      return _alphabet[21];
    case 'w':
      return _alphabet[22];
    case 'x':
      return _alphabet[23];
    case 'y':
      return _alphabet[24];
    case 'z':
      return _alphabet[25];

    // numbers
    case '0':
      return _digits[0];
    case '1':
      return _digits[1];
    case '2':
      return _digits[2];
    case '3':
      return _digits[3];
    case '4':
      return _digits[4];
    case '5':
      return _digits[5];
    case '6':
      return _digits[6];
    case '7':
      return _digits[7];
    case '8':
      return _digits[8];
    case '9':
      return _digits[9];

    // special characters: ! ? [( )] / \ *° " ' + _ : ; = > < ^ ,
    case ' ':
      return _digits[10];
    case '.':
      return _segments[7];
    case '-':
      return getMinus();
    case '!':
      return _specialCharacters[0];
    case '?':
      return _specialCharacters[1];
    case '[':
      return _specialCharacters[2];
    case '(':
      return _specialCharacters[2];
    case ']':
      return _specialCharacters[3]; 
    case ')':
      return _specialCharacters[3]; 
    case '/':
      return _specialCharacters[4];
    case '\\':
      return _specialCharacters[5];
    case '*':
      return _specialCharacters[6];
    case '°':
      return _specialCharacters[6];
    case '"':
      return _specialCharacters[7]; 
    case '\'':
      return _specialCharacters[8];
    case '+':
      return _specialCharacters[9];
    case '_': 
      return _specialCharacters[10];
    case ':':
      return _specialCharacters[11]; 
    case ';':
      return _specialCharacters[12];
    case '=':
      return _specialCharacters[13];
    case '>': 
      return _specialCharacters[14];
    case '<':
      return _specialCharacters[15]; 
    case '^':
      return _specialCharacters[16];
    case ',':
      return _specialCharacters[17];
  }

  return _unidentifiedChar;
}
byte SegmentController::getMinus()
{
  // return the minus segment
  return _segments[6];
}
byte SegmentController::getDot()
{
  // return the dot segment
  return _segments[7];
}


/*
  --- BYTES ---
*/

byte SegmentController::addByte(byte b, byte addition)
{
  // check if the display is common anode, use AND operator otherwise use OR
  if (_commonPinType == 1)
    return (byte)b & (byte)addition;
  return (byte)b | (byte)addition;
}
byte SegmentController::subtractByte(byte b, byte subtraction)
{
  // inverse the byte
  byte sub = inverseByte(subtraction);
  // check if the display is common anode, use OR operator otherwise use AND
  if (_commonPinType == 1)
    return (byte)b | (byte)sub;
  return (byte)b & (byte)sub;
}
byte SegmentController::inverseByte(byte b)
{
  // perform the inverse operation on the byte
  return ~b;
}
byte SegmentController::setByteSegment(byte b, byte segmentIndex, bool value)
{
  // return if segment is not in range
  if (!isSegmentInRange(segmentIndex))
    return b;
  // perform addition or subtraction according to the value
  if (value)
    return addByte(b, _segments[segmentIndex]);
  return subtractByte(b, _segments[segmentIndex]);
}
bool SegmentController::byteSegmentActive(byte b, byte segmentIndex)
{
  // return if segment is not in range
  if (!isSegmentInRange(segmentIndex))
    return false;

  // check if the display is common anode, use OR operator otherwise use AND; check if the segment is enabled
  if (_commonPinType == 1)
    return (b | _segments[segmentIndex]) != B11111111;
  return (b & _segments[segmentIndex]) != 0;
}


/*
  --- DISPLAY BYTES ---
*/

void SegmentController::setDigit(int digitIndex, byte b) 
{
  // check if the digit is outside of the range
  if (!isDigitInRange(digitIndex))
    return;
  // set the currently displayed byte to the given byte
  _currentDisplayByte[digitIndex] = b;
}
byte SegmentController::getDigit(int digitIndex)
{
  // check if the digit is outside of the range
  if (!isDigitInRange(digitIndex))
    return 0;
  // return the currently displayed byte
  return _currentDisplayByte[digitIndex];
}
void SegmentController::setDigitSegment(int digitIndex, byte segmentIndex, bool value)
{
  // check if the digit or segment is outside of the range
  if (!isDigitInRange(digitIndex) || !isSegmentInRange(segmentIndex))
    return;
  // set a segment of the current displayed byte
  _currentDisplayByte[digitIndex] = setByteSegment(_currentDisplayByte[digitIndex], segmentIndex, value);
}
bool SegmentController::digitSegmentActive(int digitIndex, byte segmentIndex)
{
  // check if the digit or segment is outside of the range
  if (!isDigitInRange(digitIndex) || !isSegmentInRange(segmentIndex))
    return false;
  // check if the segment of the currently displayed byte is active or not
  return byteSegmentActive(_currentDisplayByte[digitIndex], segmentIndex);
}


/*
  --- EFFECTS ---
*/

void SegmentController::setDigitState(int digitIndex, bool value)
{
  // check if the digit is outside of the range
  if (!isDigitInRange(digitIndex))
    return;
  // set the value for the digit
  _enabledDigit[digitIndex] = value;
}
void SegmentController::setDigitStateAll(bool value)
{
  // go through all digits and set the value
  for (int i = 0; i < _displayLength; i++)
    _enabledDigit[i] = value;
}
bool SegmentController::getDigitState(int digitIndex)
{
  // check if the digit is outside of the range
  if (!isDigitInRange(digitIndex))
    return false;
  // return the state of the digit
  return _enabledDigit[digitIndex];
}

void SegmentController::setBlinking(int digitIndex, bool value)
{
  // check if the digit is outside of the range
  if (!isDigitInRange(digitIndex))
    return;
  // set the value for the digit
  _blinkDigit[digitIndex] = value;
}
void SegmentController::setBlinkingAll(bool value)
{
  // go through all digits and set the value
  for (int i = 0; i < _displayLength; i++)
    _blinkDigit[i] = value;
}
bool SegmentController::getBlinking(int digitIndex)
{
  // check if the digit is outside of the range
  if (!isDigitInRange(digitIndex))
    return false;
  // return the state of the digit
  return _blinkDigit[digitIndex];
}

void SegmentController::resetEffects()
{
  // enable all digits
  setDigitStateAll(true);
  // disable blinking on all digits
  setBlinkingAll(false);
}


/*
  --- OTHER ---
*/
bool SegmentController::isStringEmpty(String s)
{
  // return if the length of the string is (lower or) equal to 0 
  return s.length() <= 0;
}
bool SegmentController::isNumberInRange(int number)
{
  // check if the number is inside of the display range
  return (-(pow(10, _displayLength - 1) - 1)) <= number && number <= (pow(10, _displayLength) - 1);
}
bool SegmentController::isNumberInRange(float number)
{
  // check if the number is inside of the display range
  return (-(pow(10, _displayLength - 1) - 1)) <= number && number <= (pow(10, _displayLength) - 1);
}
bool SegmentController::isDigitInRange(int digitIndex) 
{
  // return if the index is inside of the display range
  return 0 <= digitIndex && digitIndex < _displayLength;
}
bool SegmentController::isSegmentInRange(byte segmentIndex)
{
  // return if the index is between 0-7
  return 0 <= segmentIndex && segmentIndex < 8;
}



/*
  --- --- PRIVATE METHODS --- ---
*/

void SegmentController::setSegments(byte d)
{
  // start with the first bit (right)
  byte pointer = 1;
  // go through the segments
  for (int i = 0; i < getSegmentLength(); i++)
  {
    // perform AND operator to check if the digit should be activated or not
    byte value = d & pointer;
    // write the value from the AND operator to the pin
    digitalWrite(_segmentPins[i], value);
    // go to the next bit (left)
    pointer *= 2;
  }
}
void SegmentController::updateBrightness()
{
  // inverse the brightness if the display is common anode
  if (_commonPinType == 1)
    _brightness = 255 - _brightness;
}
void SegmentController::transformDigit(int digitIndex, int shift)
{
  // get the index to copy from
  int index = digitIndex + shift;

  // check if the copy index is in range and copy, if not copy empty digit
  if (isDigitInRange(index))
    _currentDisplayByte[digitIndex] = _currentDisplayByte[index];
  else
    _currentDisplayByte[digitIndex] = _digits[10];
}

int SegmentController::getSegmentLength()
{
  // return the length of the segments according to the decimal pin
  if (_hasDP)
    return 8;
  return 7;
}
bool SegmentController::hasText()
{
  // return if the display string is not empty
  return !isStringEmpty(_displayText);
}

void SegmentController::updateDisplayText()
{
  // calculate a duration to prevent millis overflow
  if (millis() - _lastTextChange > _textUpdateTime)
  {
    // transform the text by one
    transform(-1);
    // select the next element
    _displayTextIndex++;

    // reset the index if the text is done scrolling
    if (_displayTextIndex >= _displayTextLength + _displayLength)
      _displayTextIndex = -1;
    // set the next text element if it is still in range
    else if (_displayTextIndex < _displayTextLength)
      _currentDisplayByte[0] = getCharacter(_displayText[_displayTextIndex]);

    // set the last timestamp to the current milliseconds
    _lastTextChange = millis();
  }
}
void SegmentController::clearDisplayText()
{
  // reset the string
  _displayText = "";
}
