/*
  DonutStudioSevenSegment.h - Library for controlling a seven-segment-display with n digits.
  Created by Donut Studio, December 21, 2022.
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
  --- SEGMENT CONTROLLER CLASS ---
*/

SegmentController::SegmentController(int a, int b, int c, int d, int e, int f, int g, int dp, int gnd[], int length)
{
  segmentPins[0] = a;
  segmentPins[1] = b;
  segmentPins[2] = c;
  segmentPins[3] = d;
  segmentPins[4] = e;
  segmentPins[5] = f;
  segmentPins[6] = g;
  segmentPins[7] = dp;
  hasDP = dp > 0;
  
  displayLength = length;
  if (displayLength > MAXDIGITS)
    displayLength = MAXDIGITS;

  for (int i = 0; i < displayLength; i++)
    gndPins[i] = gnd[i];
}

/*
  --- PUBLIC METHODS ---
*/

void SegmentController::initialize(bool _commonAnode, unsigned long _refreshTime, byte _brightness)
{
  commonPinType = _commonAnode ? 1 : 0;
  refreshTime = _refreshTime;
  setBrightness(_brightness);

  if (commonPinType == 1) 
  {
    for (int i = 0; i < sizeof(digits) / sizeof(digits[0]); i++) 
      digits[i] = inverseByte(digits[i]);
    minus = inverseByte(minus);
    dot = inverseByte(dot);
  }


  for (int i = 0; i < getSegmentLength(); i++){
    pinMode(segmentPins[i], OUTPUT);
    digitalWrite(segmentPins[i], 1 - commonPinType);
  }

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

void SegmentController::setByte(byte _digits[])
{
  for (int i = 0; i < displayLength; i++)
    currentDisplayByte[i] = _digits[i];
}
void SegmentController::setInt(int _number, bool _showLeadZeros)
{
  if (!NumberInRange(_number))
    return;
  
  bool lead = !_showLeadZeros;
  for (int i = displayLength - 1; i >= 0 ; i--)
  {
    byte digit = (int)(_number / pow(10, i)) % 10;
    if (digit == 0 && lead)
    {
      currentDisplayByte[i] = digits[10];
      continue;
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
  if (!NumberInRange(_number))
    return;

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
      currentDisplayByte[i] = addDot(digits[digit]);
    else
      currentDisplayByte[i] = digits[digit];
  }
}

byte SegmentController::getDigit(int _digit)
{
  if (_digit < 0 || _digit > 9)
    return digits[11];
  return digits[_digit];
}
byte SegmentController::getMinus()
{
  return minus;
}
byte SegmentController::addDot(byte _byte)
{
  if (commonPinType == 1)
    return _byte & (byte)dot;
  else 
    return _byte | (byte)dot;
}
byte SegmentController::inverseByte(byte _byte)
{
  return ~_byte;
}

void SegmentController::setBrightness(byte _brightness)
{
  brightness = _brightness;
}
void SegmentController::setBlinkInterval(int _blinkInterval)
{
  blinkInterval = _blinkInterval;
}

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

int SegmentController::getSegmentLength()
{
  if (hasDP)
    return 8;
  return 7;
}
bool SegmentController::NumberInRange(int _number)
{
  return (-(pow(10, displayLength - 1) - 1)) <= _number 
    && _number <= (pow(10, displayLength) - 1);
}
bool SegmentController::NumberInRange(float _number)
{
  return (-(pow(10, displayLength - 1) - 1)) <= _number 
    && _number <= (pow(10, displayLength) - 1);
}