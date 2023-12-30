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

#include "Arduino.h"
#include "DonutStudioSevenSegment.h"

/*
  --- CONSTRUCTOR ---
*/

SegmentController::SegmentController(bool commonAnode, int segmentPins[8], int digitPins[], byte displayLength, byte refreshTime)
{
  _commonPinType = commonAnode ? 1 : 0;
  _hasDP = segmentPins[7] > 0;
  _refreshTime = refreshTime;

  _displayLength = displayLength;
  if (_displayLength > MAXDIGITS)
    _displayLength = MAXDIGITS;


  // set the segment pins
  for (int i = 0; i < 8; i++)
  {
    _segmentPins[i] = segmentPins[i];
    pinMode(_segmentPins[i], OUTPUT);
    digitalWrite(_segmentPins[i], 1 - _commonPinType);
  }
  
  // set the digit pins
  for (int i = 0; i < _displayLength; i++)
  {
    _digitPins[i] = digitPins[_displayLength - 1 - i];
    pinMode(_digitPins[i], OUTPUT);
    digitalWrite(_digitPins[i], 1 - _commonPinType);

    _digitBlinking[i] = false;
    _digitEnabled[i] = true;
    _displayedBytes[i] = _digits[10];
  }
}


/*
  --- PUBLIC METHODS ---
*/

/*-- MAIN --*/

void SegmentController::refresh()
{
  if (_isScrolling)
    updateScroller();

  for (int i = 0; i < _displayLength; i++)
  {
    if (!_digitEnabled[_displayLength - 1 - i])
      continue;
    else if (_digitBlinking[_displayLength - 1 - i] && (millis() % (_blinkInterval * 2) > _blinkInterval))
    {
      delay(_refreshTime);
      continue;
    }
    
    // enable
    analogWrite(_digitPins[i],  isCommonAnode() ? (_brightness) : (byte)(255 - _brightness));
    setSegments(_displayedBytes[i]);
    
    delay(_refreshTime);
    
    // disable
    analogWrite(_digitPins[i], 255 * (1 - _commonPinType));
    setSegments(_digits[10]);
  }

  delay(_refreshTime);
}
void SegmentController::clear()
{
  disableScroller();
  setSegments(_digits[10]);
  for (int i = 0; i < _displayLength; i++)
  {
    analogWrite(_digitPins[i], 255 * (1 - _commonPinType));
    _displayedBytes[i] = _digits[10];
  }
}
void SegmentController::transform(int shift)
{
  if (shift == 0)
    return;

  // positive shift -> right
  if (shift > 0) 
    for (int i = 0; i < _displayLength; i++)
      transformDigit(i, shift);
  // negative shift -> left
  else
    for (int i = _displayLength - 1; i >= 0; i--)
      transformDigit(i, shift);
}
void SegmentController::setBrightness(byte brightness)
{
  _brightness = brightness;
}
byte SegmentController::getBrightness()
{
  return _brightness;
}


/*-- DISPLAY --*/

void SegmentController::setByte(byte b[])
{
  disableScroller();

  for (int i = 0; i < _displayLength; i++)
    _displayedBytes[i] = b[_displayLength - i - 1];
}
void SegmentController::setInt(int number, bool showLeadZeros)
{
  if (!isNumberInRange(number))
    return;
  disableScroller();
  
  bool lead = !showLeadZeros;
  bool negativ = number < 0;
  if (negativ)
    number *= -1;

  for (int i = _displayLength - 1; i >= 0 ; i--)
  {
    if (negativ && !lead && i == _displayLength - 1)
    {
      _displayedBytes[i] = getMinus();
      negativ = false;
      continue;
    }

    byte digit = (int)(number / pow(10, i)) % 10;
    if (negativ && digit == 0 && i > 0)
    {
      byte d = (int)(number / pow(10, i - 1)) % 10;
      if (d != 0)
      {
        _displayedBytes[i] = getMinus();
        negativ = false;
      }
    }
    else if (lead && digit == 0)
      _displayedBytes[i] = _digits[10];
    else
    {
      _displayedBytes[i] = _digits[digit];
      lead = false;
    }
  }
}

/*
void SegmentController::setFloat(float number)
{
  if (!isNumberInRange(number))
    return;
  disableScroller();

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
  if (exponent > 0)
    dotIndex -= exponent;

  setInt(round(number * pow(10, dotIndex)));
  setDigitSegment(_displayLength - 1 - dotIndex, 7, true);
  if (negativ)
    setDigit(0, getMinus());
}
*/

void SegmentController::setString(String text, int shift) 
{
  if (isStringEmpty(text))
    return;
  disableScroller();

  for (int i = _displayLength - 1; i >= 0 ; i--)
  {
    int index = (_displayLength - 1) - i;
    index += shift;

    if (index < 0 || index >= text.length())
      _displayedBytes[i] = _digits[10];
    else 
      _displayedBytes[i] = getCharacter(text[index]);
  }
}

void SegmentController::setDigit(byte digitIndex, byte b) 
{
  if (!isDigitInRange(digitIndex))
    return;
  _displayedBytes[_displayLength - digitIndex- 1] = b;
}
byte SegmentController::getDigit(byte digitIndex)
{
  if (!isDigitInRange(digitIndex))
    return 0;
  return _displayedBytes[_displayLength - digitIndex - 1];
}
void SegmentController::setDigitSegment(byte digitIndex, byte segmentIndex, bool value)
{
  if (!isDigitInRange(digitIndex) || !isSegmentInRange(segmentIndex))
    return;
  _displayedBytes[_displayLength - digitIndex - 1] = setSegment(_displayedBytes[_displayLength - digitIndex - 1], segmentIndex, value);
}
bool SegmentController::digitSegmentActive(byte digitIndex, byte segmentIndex)
{
  if (!isDigitInRange(digitIndex) || !isSegmentInRange(segmentIndex))
    return false;
  return isSegmentActive(_displayedBytes[_displayLength - digitIndex - 1], segmentIndex);
}


/*-- SCROLLER --*/

void SegmentController::setScroller(String text)
{
  if (isStringEmpty(text))
    return;
  clear();

  _isScrolling = true;
  _scrollerLength = text.length();
  if(_scrollerLength > MAXSCROLLERSIZE)
    _scrollerLength = MAXSCROLLERSIZE;
  
  _currentScrollIndex = -1;
  _previousScrollTime = millis();

  for (int i = 0; i < _scrollerLength; i++)
  {
    _scroller[i] = getCharacter(text[i]);
  }
}
void SegmentController::setScroller(byte bytes[], int size)
{
  clear();
  _isScrolling = true;
  _scrollerLength = size;
  if(_scrollerLength > MAXSCROLLERSIZE)
    _scrollerLength = MAXSCROLLERSIZE;
  
  _currentScrollIndex = -1;
  _previousScrollTime = millis();

  for (int i = 0; i < _scrollerLength; i++)
  {
    _scroller[i] = bytes[i];
  }
}
void SegmentController::appendScroller(String text)
{
  if (isStringEmpty(text))
    return;
  if (!_isScrolling)
  {
    setScroller(text);
    return;
  }

  int l = text.length();
  _scrollerLength += l;

  _previousScrollTime = millis();

  for (int i = _scrollerLength - l; i < _scrollerLength; i++)
  {
    if (i > MAXSCROLLERSIZE)
    {
      _scrollerLength = MAXSCROLLERSIZE;
      return;
    }
      
    _scroller[i] = getCharacter(text[i - _scrollerLength + l]);
  }
}
void SegmentController::appendScroller(byte bytes[], int size)
{
  if (!_isScrolling)
  {
    setScroller(bytes, size);
    return;
  }

  _scrollerLength += size;
  _previousScrollTime = millis();

  for (int i = _scrollerLength - size; i < _scrollerLength; i++)
  {
    if (i > MAXSCROLLERSIZE)
    {
      _scrollerLength = MAXSCROLLERSIZE;
      return;
    }
      
    _scroller[i] = bytes[i - _scrollerLength + size];
  }
}

void SegmentController::setScrollElement(byte index, byte b)
{
  if (index < 0 && index >= _scrollerLength)
    return;
  _scroller[index] = b;
}
byte SegmentController::getScrollElement(byte index)
{
  if (index < 0 && index >= _scrollerLength)
    return 0;
  return _scroller[index];
}
byte SegmentController::getScrollerLength()
{
  return _scrollerLength;
}
void SegmentController::setScrollerUpdateTime(unsigned int updateTime)
{
  _scrollUpdateTime = updateTime;
}
unsigned int SegmentController::getScrollerUpdateTime()
{
  return _scrollUpdateTime;
}


/*-- GET --*/

byte SegmentController::getNumber(int number)
{
  if (number < 0 || number > 9)
    return _digits[10];
  return _digits[number];
}
byte SegmentController::getCharacter(char character)
{
  character = tolower(character);

  if (isAlpha(character))
    return _alphabet[(character - 97)];
  if (isDigit(character))
    return _digits[(character - 48)];

  switch (character)
  {
    // special characters: , ! ? = > < ( ) / \ * " ^
    case '-':
      return getMinus();
    case '.':
      return getDot();
    case '_':
      return getSegment(3);
    case '\'':
      return getSegment(5);
    case ',':
      return _specialCharacters[0];
    case '!':
      return _specialCharacters[1];
    case '?':
      return _specialCharacters[2];
    case '=':
      return _specialCharacters[3];
    case '>':
      return _specialCharacters[4];
    case '<':
      return _specialCharacters[5];
    case '(':
      return _specialCharacters[6];
    case ')':
      return _specialCharacters[7];
    case '/':
      return _specialCharacters[8];
    case '\\':
      return _specialCharacters[9];
    case '*':
      return _specialCharacters[10];
    case '°':
      return _specialCharacters[10];
    case '"':
      return _specialCharacters[11];
    case '^':
      return _specialCharacters[12];
  }

  return _digits[10];
}
byte SegmentController::getMinus()
{
  return getSegment(6);
}
byte SegmentController::getDot()
{
  return getSegment(7);
}
byte SegmentController::getSegment(byte segmentIndex)
{
  if (!isSegmentInRange(segmentIndex))
    return 0;
  return round(pow(2, segmentIndex));
}


/*-- BYTES --*/

byte SegmentController::combineBytes(byte byte1, byte byte2)
{
  return byte1 | byte2;
}
byte SegmentController::subtractBytes(byte byte1, byte subtraction)
{
  return byte1 & (~subtraction);
}
byte SegmentController::setSegment(byte byte1, byte segmentIndex, bool value)
{
  if (!isSegmentInRange(segmentIndex))
    return byte1;
    
  if (value)
    return combineBytes(byte1, getSegment(segmentIndex));
  return subtractBytes(byte1, getSegment(segmentIndex));
}
bool SegmentController::isSegmentActive(byte byte1, byte segmentIndex)
{
  if (!isSegmentInRange(segmentIndex))
    return false;

  return (byte1 & getSegment(segmentIndex)) != 0;
}


/*-- EFFECTS --*/

void SegmentController::setDigitState(byte digitIndex, bool value)
{
  if (!isDigitInRange(digitIndex))
    return;
  _digitEnabled[digitIndex] = value;
}
void SegmentController::setDigitStateAll(bool value)
{
  for (int i = 0; i < _displayLength; i++)
    _digitEnabled[i] = value;
}
bool SegmentController::getDigitState(byte digitIndex)
{
  if (!isDigitInRange(digitIndex))
    return false;
  return _digitEnabled[digitIndex];
}

void SegmentController::setBlinking(byte digitIndex, bool value)
{
  if (!isDigitInRange(digitIndex))
    return;
  _digitBlinking[digitIndex] = value;
}
void SegmentController::setBlinkingAll(bool value)
{
  for (int i = 0; i < _displayLength; i++)
    _digitBlinking[i] = value;
}
bool SegmentController::getBlinking(byte digitIndex)
{
  if (!isDigitInRange(digitIndex))
    return false;
  return _digitBlinking[digitIndex];
}

void SegmentController::setBlinkInterval(unsigned int blinkInterval)
{
  _blinkInterval = blinkInterval;
}
unsigned int SegmentController::getBlinkInterval()
{
  return _blinkInterval;
}

void SegmentController::resetEffects()
{
  setDigitStateAll(true);
  setBlinkingAll(false);
}


/*-- CHECKS --*/

bool SegmentController::isNumberInRange(int number)
{
  return (-(pow(10, _displayLength - 1) - 1)) <= number && number <= (pow(10, _displayLength) - 1);
}
bool SegmentController::isNumberInRange(float number)
{
  return (-(pow(10, _displayLength - 1) - 1)) <= number && number <= (pow(10, _displayLength) - 1);
}
bool SegmentController::isDigitInRange(byte digitIndex) 
{
  return 0 <= digitIndex && digitIndex < _displayLength;
}
bool SegmentController::isSegmentInRange(byte segmentIndex)
{
  return 0 <= segmentIndex && segmentIndex < 8;
}



/*
  --- PRIVATE METHODS ---
*/

void SegmentController::setSegments(byte d)
{
  // start with the first bit (right)
  byte pointer = 1;
  for (int i = 0; i < getSegmentLength(); i++)
  {
    // enable/disable the segment pin
    bool value = (d & pointer) != 0;
    digitalWrite(_segmentPins[i], isCommonAnode() ? !value : value);
    // move one bit to the left
    pointer *= 2;
  }
}
void SegmentController::transformDigit(byte digitIndex, int shift)
{
  int index = digitIndex + shift;

  if (isDigitInRange(index))
    _displayedBytes[digitIndex] = _displayedBytes[index];
  else
    _displayedBytes[digitIndex] = _digits[10];
}

int SegmentController::getSegmentLength()
{
  if (_hasDP)
    return 8;
  return 7;
}
bool SegmentController::isCommonAnode()
{
  return _commonPinType == 1;
}
bool SegmentController::isStringEmpty(String s)
{
  return s.length() <= 0;
}

void SegmentController::updateScroller()
{
  if (millis() - _previousScrollTime > _scrollUpdateTime)
  {
    transform(-1);
    _currentScrollIndex++;

    // reset the index if the text is done scrolling
    if (_currentScrollIndex >= _scrollerLength + _displayLength)
      _currentScrollIndex = -1;

    // set the next text element
    else if (_currentScrollIndex < _scrollerLength)
      _displayedBytes[0] = _scroller[_currentScrollIndex];

    _previousScrollTime = millis();
  }
}
void SegmentController::disableScroller()
{
  _isScrolling = false;
}