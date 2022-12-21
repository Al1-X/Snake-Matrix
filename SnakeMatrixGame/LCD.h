#ifndef LCD_H
#define LCD_H

#include "WString.h"
#include <LiquidCrystal.h>
#include <Arduino.h>

const byte RS = 13;
const byte enable = 8;
const byte d4 = 7;
const byte d5 = 6;
const byte d6 = 3;
const byte d7 = 4;

const byte blackLightPin = 5;

LiquidCrystal lcd(RS, enable, d4, d5, d6, d7);

void printText(int printStart, int lineIndex, char* text)
{
  lcd.setCursor(printStart, lineIndex);
  lcd.print(text);
}

void printLongText(int printStart, int lineIndex, char* text, unsigned int timeDelay)
{
  for (int startLetter = 0; startLetter <= strlen(text) - 16; startLetter++)
  {
    lcd.setCursor(printStart, lineIndex);
    for (int letter = startLetter; letter <= startLetter + 15; letter++)
    {
      lcd.print(text[letter]);
    }
    lcd.print(" ");
    delay(timeDelay);
  }
}

#endif
