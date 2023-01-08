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

#include "Arduino.h"
#include "DonutStudioSevenSegment.h"

/*
  --- SEGMENT CONTROLLER CONSTRUCTOR ---
*/
SegmentController::SegmentController(int a, int b, int c, int d, int e, int f, int g, int dp, int gnd[], int length)
{
  // set the segment pins
  segmentPins[0] = a;
  segmentPins[1] = b;
  segmentPins[2] = c;
  segmentPins[3] = d;
  segmentPins[4] = e;
  segmentPins[5] = f;
  segmentPins[6] = g;
  segmentPins[7] = dp;
  // check if the dp will be used
  hasDP = dp > 0;
  
  // set the display length
  displayLength = length;
  if (displayLength > MAXDIGITS)
    displayLength = MAXDIGITS;

  // set the ground pins
  for (int i = 0; i < displayLength; i++)
    gndPins[i] = gnd[i];
}

/*
  --- PUBLIC METHODS ---
*/
void SegmentController::initialize(bool _commonAnode, unsigned long _refreshTime, byte _brightness)
{
  // set the common pin type 
  commonPinType = _commonAnode ? 1 : 0;
  // set the display refresh time
  refreshTime = _refreshTime;
  // set the brightness of the display
  setBrightness(_brightness);

  // inverse the bytes if the display is common anode
  if (commonPinType == 1) 
  {
    // inverse digits
    for (int i = 0; i < sizeof(digits) / sizeof(digits[0]); i++) 
      digits[i] = inverseByte(digits[i]);
    // inverse segments
    for (int i = 0; i < sizeof(segments) / sizeof(segments[0]); i++)
      segments[i] = inverseByte(segments[i]);
    // inverse the alphabet
    for (int i = 0; i< sizeof(alphabet) / sizeof(alphabet[0]); i ++)
      alphabet[i] = inverseByte(alphabet[i]);
    // inverse the special characters
    for (int i = 0; i< sizeof(specialCharacters) / sizeof(specialCharacters[0]); i ++)
      specialCharacters[i] = inverseByte(specialCharacters[i]);
    // inverse the unknown byte
    unknownChar = inverseByte(unknownChar);
  }

  // set the segment pins as output and deactivate them
  for (int i = 0; i < getSegmentLength(); i++)
  {
    pinMode(segmentPins[i], OUTPUT);
    digitalWrite(segmentPins[i], 1 - commonPinType);
  }

  // set the ground pins as output and deactivate them, reset the effects and displayment byte
  for (int i = 0; i < displayLength; i++)
  {
    blinkDigit[i] = false;
    enabledDigit[i] = true;
    currentDisplayByte[i] = digits[10];
    pinMode(gndPins[i], OUTPUT);
    digitalWrite(gndPins[i], 1 - commonPinType);
  }
}
void SegmentController::refreshDisplay()
{
  updateBrightness();
  if (hasText())
    updateDisplayText();

  for (int i = 0; i < displayLength; i++)
  {
    if (!enabledDigit[i])
      continue;
    else if (blinkDigit[i] && (millis() % (blinkInterval * 2) > blinkInterval))
    {
      delay(refreshTime);
      continue;
    }
    
    analogWrite(gndPins[i], (byte)(255 - constrain(brightness, 0, 255)));
    setSegments(currentDisplayByte[i]);
    
    delay(refreshTime);
    
    analogWrite(gndPins[i], 255 * (1 - commonPinType));
    setSegments(digits[10]);
  }
  delay(refreshTime);
}
void SegmentController::clearDisplay()
{
  for (int i = 0; i < displayLength; i++)
    currentDisplayByte[i] = digits[10];
  clearDisplayText();
}

/*
  settings
*/
void SegmentController::setBrightness(byte _brightness)
{
  brightness = _brightness;
}
void SegmentController::setBlinkInterval(unsigned int _blinkInterval)
{
  blinkInterval = _blinkInterval;
}
void SegmentController::setUnknownChar(byte _byte)
{
  unknownChar = _byte;
}
void SegmentController::setTextSpeed(unsigned int _speed)
{
  textSpeed = _speed;
}

/*
  displayment
*/
void SegmentController::setByte(byte _digits[])
{
  clearDisplayText();
  for (int i = 0; i < displayLength; i++)
    currentDisplayByte[i] = _digits[i];
}
void SegmentController::setInt(int _number, bool _showLeadZeros)
{
  if (!numberInRange(_number))
    return;
  clearDisplayText();
  
  bool lead = !_showLeadZeros;
  bool negativ = _number < 0;
  if (negativ)
    _number *= -1;

  for (int i = displayLength - 1; i >= 0 ; i--)
  {
    if (negativ && !lead && i == displayLength - 1)
    {
      currentDisplayByte[i] = getMinus();
      negativ = false;
      continue;
    }

    byte digit = (int)(_number / pow(10, i)) % 10;
    if (negativ && digit == 0 && i > 0)
    {
      byte d = (int)(_number / pow(10, i - 1)) % 10;
      if (d != 0)
      {
        currentDisplayByte[i] = getMinus();
        negativ = false;
      }
    }
    else if (lead && digit == 0)
    {
      currentDisplayByte[i] = digits[10];
    }
    else
    {
      currentDisplayByte[i] = digits[digit];
      lead = false;
    }
  }
}
void SegmentController::setFloat(float _number)
{
  if (!numberInRange(_number))
    return;
  clearDisplayText();

  bool negativ = _number < 0;
  if (negativ)
    _number *= -1;

  int exponent = 0;
  for (int i = -displayLength + 2; i <= displayLength; i++)
  {
    if (_number < pow(10, i))
    {
      exponent = i - 1;
      break;
    }
  }
  
  int dotIndex = displayLength - 1;
  if(exponent > 0)
    dotIndex -= exponent;

  _number = _number * pow(10, dotIndex);
  for (int i = 0; i < displayLength; i++)
  {
    byte digit = (int)(_number / pow(10, i)) % 10;

    if (i == dotIndex)
      currentDisplayByte[i] = setSegment(digits[digit], 7, true);
    else
      currentDisplayByte[i] = digits[digit];
  }
}
void SegmentController::setString(String _string, int _transform) 
{
  if (isStringEmpty(_string))
    return;
  if (displayText != "" && !displayText.equals(_string))
    clearDisplayText();

  for (int i = displayLength - 1; i >= 0 ; i--)
  {
    int index = (displayLength - 1) - i;
    index += _transform;

    if (index < 0 || index >= _string.length())
      currentDisplayByte[i] = digits[10];
    else 
      currentDisplayByte[i] = getCharacter(_string[index]);
  }
}
void SegmentController::setText(String _text, bool restart) 
{
  if (isStringEmpty(_text))
    return;
  displayText = _text;
  displayTextLength = displayText.length();

  if (textTransform >= displayTextLength)
    restart = true;
  if (restart)
    textTransform = -displayLength;

  textChangeInterval = millis() + textSpeed;

  setString(displayText, textTransform);
}

/*
  byte manipulation
*/
byte SegmentController::getDigit(int _digit)
{
  if (_digit < 0 || _digit > 9)
    return digits[10];
  return digits[_digit];
}
byte SegmentController::getCharacter(char _character)
{
  _character = tolower(_character);
  switch (_character)
  {
    // alphabet
    case 'a':
      return alphabet[0];
    case 'b':
      return alphabet[1];
    case 'c':
      return alphabet[2];
    case 'd':
      return alphabet[3];
    case 'e':
      return alphabet[4];
    case 'f':
      return alphabet[5];
    case 'g':
      return alphabet[6];
    case 'h':
      return alphabet[7];
    case 'i':
      return alphabet[8];
    case 'j':
      return alphabet[9];
    case 'k':
      return alphabet[10];
    case 'l':
      return alphabet[11];
    case 'm':
      return alphabet[12];
    case 'n':
      return alphabet[13];
    case 'o':
      return alphabet[14];
    case 'p':
      return alphabet[15];
    case 'q':
      return alphabet[16];
    case 'r':
      return alphabet[17];
    case 's':
      return alphabet[18];
    case 't':
      return alphabet[19];
    case 'u':
      return alphabet[20];
    case 'v':
      return alphabet[21];
    case 'w':
      return alphabet[22];
    case 'x':
      return alphabet[23];
    case 'y':
      return alphabet[24];
    case 'z':
      return alphabet[25];

    // numbers
    case '0':
      return digits[0];
    case '1':
      return digits[1];
    case '2':
      return digits[2];
    case '3':
      return digits[3];
    case '4':
      return digits[4];
    case '5':
      return digits[5];
    case '6':
      return digits[6];
    case '7':
      return digits[7];
    case '8':
      return digits[8];
    case '9':
      return digits[9];

    // special characters: ! ? [( )] / \ *° " ' + _ : ; = > < ^ ,
    case ' ':
      return digits[10];
    case '.':
      return segments[7];
    case '-':
      return getMinus();
    case '!':
      return specialCharacters[0];
    case '?':
      return specialCharacters[1];
    case '[':
      return specialCharacters[2];
    case '(':
      return specialCharacters[2];
    case ']':
      return specialCharacters[3]; 
    case ')':
      return specialCharacters[3]; 
    case '/':
      return specialCharacters[4];
    case '\\':
      return specialCharacters[5];
    case '*':
      return specialCharacters[6];
    case '°':
      return specialCharacters[6];
    case '"':
      return specialCharacters[7]; 
    case '\'':
      return specialCharacters[8];
    case '+':
      return specialCharacters[9];
    case '_': 
      return specialCharacters[10];
    case ':':
      return specialCharacters[11]; 
    case ';':
      return specialCharacters[12];
    case '=':
      return specialCharacters[13];
    case '>': 
      return specialCharacters[14];
    case '<':
      return specialCharacters[15]; 
    case '^':
      return specialCharacters[16];
    case ',':
      return specialCharacters[17];
  }

  return unknownChar;
}
byte SegmentController::getMinus()
{
  return segments[6];
}
byte SegmentController::getDot()
{
  return segments[7];
}

byte SegmentController::addByte(byte _byte, byte _addition)
{
  if (commonPinType == 1)
    return (byte)_byte & (byte)_addition;
  return (byte)_byte | (byte)_addition;
}
byte SegmentController::subtractByte(byte _byte, byte _subtraction)
{
  byte subt = inverseByte(_subtraction);
  if (commonPinType == 1)
    return (byte)_byte | (byte)subt;
  return (byte)_byte & (byte)subt;
}
byte SegmentController::setSegment(byte _byte, int _segment, bool _value)
{
  if (_segment < 0 || _segment > 7)
    return _byte;
  if (_value)
    return addByte(_byte, segments[_segment]);
  return subtractByte(_byte, segments[_segment]);
}

byte SegmentController::inverseByte(byte _byte)
{
  return ~_byte;
}

void SegmentController::setDisplaySegment(int _digit, int _segment, bool _value)
{
  if (_digit < 0 || _digit >= displayLength)
    return;
  currentDisplayByte[_digit] = setSegment(currentDisplayByte[_digit], _segment, _value);
}

/*
  effects
*/
void SegmentController::setBlink(int _digit, bool _value)
{
  if (_digit < 0 || _digit >= displayLength)
    return;
  blinkDigit[_digit] = _value;
}
void SegmentController::setBlinkAll(bool _value)
{
  for (int i = 0; i < displayLength; i++)
    blinkDigit[i] = _value;
}
void SegmentController::setDigit(int _digit, bool _value)
{
  if (_digit < 0 || _digit >= displayLength)
    return;
  enabledDigit[_digit] = _value;
}
void SegmentController::setDigitAll(bool _value)
{
  for (int i = 0; i < displayLength; i++)
    enabledDigit[i] = _value;
}
void SegmentController::resetEffects()
{
  setDigitAll(true);
  setBlinkAll(false);
}

/*
  other
*/
bool SegmentController::isStringEmpty(String _string)
{
  return _string.length() <= 0;
}
bool SegmentController::numberInRange(int _number)
{
  return (-(pow(10, displayLength - 1) - 1)) <= _number 
    && _number <= (pow(10, displayLength) - 1);
}
bool SegmentController::numberInRange(float _number)
{
  return (-(pow(10, displayLength - 1) - 1)) <= _number 
    && _number <= (pow(10, displayLength) - 1);
}

/*
  --- PRIVATE METHODS ---
*/
void SegmentController::setSegments(byte _digit)
{
  byte pointer = 1;

  for (int i = 0; i < getSegmentLength(); i++)
  {
    byte value = _digit & pointer;
    digitalWrite(segmentPins[i], value);
    pointer *= 2;
  }
}
void SegmentController::updateBrightness()
{
  if (commonPinType == 1)
    brightness = 255 - brightness;
}
void SegmentController::updateDisplayText()
{
  if (millis() >= textChangeInterval)
  {
    textTransform++;
    if (textTransform >= displayTextLength)
      textTransform = -displayLength;

    textChangeInterval = millis() + textSpeed;

    setString(displayText, textTransform);
  }
}
void SegmentController::clearDisplayText()
{
  displayText = "";
}

int SegmentController::getSegmentLength()
{
  if (hasDP)
    return 8;
  return 7;
}
bool SegmentController::hasText()
{
  return !isStringEmpty(displayText);
}