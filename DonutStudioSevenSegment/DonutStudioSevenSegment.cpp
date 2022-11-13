/*
  DonutStudioSevenSegment.h - Library for controlling a seven-segment-display with n digits.
  Created by Donut Studio, November 13, 2022.
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

SegmentController::SegmentController(int a, int b, int c, int d, int e, int f, int g, int dp, int gnd[MAXDIGITS])
{
  segmentPins[0] = a;
  segmentPins[1] = b;
  segmentPins[2] = c;
  segmentPins[3] = d;
  segmentPins[4] = e;
  segmentPins[5] = f;
  segmentPins[6] = g;
  segmentPins[7] = dp;
  for (int i = 0; i < getSegmentLength(); i++){
    pinMode(segmentPins[i], OUTPUT);
    digitalWrite(segmentPins[i], LOW);
  }
    
  for (int i = 0; i < sizeof(gndPins) / sizeof(gndPins[0]); i++)
  {
    if (gnd[i] <= 0)
      break;
    gndPins[i] = gnd[i];
    pinMode(gndPins[i], OUTPUT);
    digitalWrite(gndPins[i], LOW);

    currentDisplayByte[i] = 255 * (1 - commonPinType);
  }
}

/*
  --- PUBLIC METHODS ---
*/

void SegmentController::initialize(bool _commonAnode, int _refreshTime = 2, byte _brightness = 175)
{
  commonPinType = _commonAnode ? 1 : 0;
  refreshTime = _refreshTime;
  brightness = _brightness;

  for (int i = 0; i < MAXDIGITS; i++)
  {
    blinkDigit[i] = false;
    enabledDigit[i] = true;
  }
  
  if (commonPinType == 1) 
  {
    for (int i = 0; i < sizeof(digits) / sizeof(digits[0]); i++) 
      digits[i] = ~digits[i];
    minus = ~minus;
    dot = ~dot;
  }
}

void SegmentController::refreshDisplay()
{
  updateBrightness();

  for (int i = 0; i < MAXDIGITS; i++)
  {
    if (!enabledDigit[i] || currentDisplayByte[i] == digits[10])
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

void SegmentController::setByte(byte d[MAXDIGITS])
{
  for (int i = 0; i < MAXDIGITS; i++)
    currentDisplayByte[i] = d[i];
}
void SegmentController::setInt(int number, bool showLeadZeros = false)
{
  if (!NumberInRange(number))
    return;
  
  bool lead = !showLeadZeros;
  for (int i = MAXDIGITS - 1; i >= 0 ; i--)
  {
    byte digit = (int)(number / pow(10, i)) % 10;
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
void SegmentController::setFloat(float number)
{
  if (!NumberInRange(number))
    return;

  int exponent = 0;
  for (int i = -MAXDIGITS + 2; i <= MAXDIGITS; i++)
  {
    if (number < pow(10, i))
    {
      exponent = i - 1;
      break;
    }
  }
  
  int dotIndex = MAXDIGITS - 1;
  if(exponent > 0)
    dotIndex -= exponent;

  number = number * pow(10, dotIndex);
  for (int i = 0; i < MAXDIGITS; i++)
  {
    byte digit = (int)((int)(number / pow(10, i)) % 10);

    if (i == dotIndex)
    {
      if (commonPinType == 1)
        currentDisplayByte[i] = (byte)digits[digit] & (byte)dot;
      else 
        currentDisplayByte[i] = (byte)digits[digit] | (byte)dot;
    }
    else
      currentDisplayByte[i] = digits[digit];
  }
}

void SegmentController::setBlinkDigit(int _digit, bool _value)
{
  if (_digit < 0 || _digit >= MAXDIGITS)
    return;
  blinkDigit[_digit] = _value;
}
void SegmentController::setBlinkAll(bool _value)
{
  for (int i = 0; i < MAXDIGITS; i++)
    blinkDigit[i] = _value;
}
void SegmentController::setDigit(int _digit, bool _value)
{
  if (_digit < 0 || _digit >= MAXDIGITS)
    return;
  enabledDigit[_digit] = _value;
}
void SegmentController::setDigitAll(bool _value)
{
  for (int i = 0; i < MAXDIGITS; i++)
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

void SegmentController::setSegments(byte digit)
{
  byte pointer = 1;

  for (int i = 0; i < getSegmentLength(); i++)
  {
    byte value = digit & pointer;
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
  return sizeof(segmentPins) / sizeof(segmentPins[0]);
}
bool SegmentController::NumberInRange(int _number)
{
  return (-(pow(10, MAXDIGITS - 1) - 1)) <= _number && _number <= (pow(10, MAXDIGITS) - 1);
}
bool SegmentController::NumberInRange(float _number)
{
  return (-(pow(10, MAXDIGITS - 1) - 1)) <= _number && _number <= (pow(10, MAXDIGITS) - 1);
}