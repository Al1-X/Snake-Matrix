#include "WString.h"
#include "HardwareSerial.h"
#ifndef SCROLL_H
#define SCROLL_H

#include "Variables.h"
#include <Arduino.h>

bool scrollActive = false;
bool joyMoved = false;

/*
- cycle through all the optioon in the array
- for the first option, show title Main Menu on the first line
- cant go back to the begining from the last option
- title has different behavior
*/


void scrollWheel(const char* tabArray[], const unsigned int arrayLenght) {
  static char* firstTab = tabArray[0];
  static char* secondTab = tabArray[1];

  static byte lineIndex = 1;

  static unsigned int pageIndex = 0;

  if (scrollActive == false) {
    firstTab = tabArray[0];
    secondTab = tabArray[1];
    lineIndex = 1;
    pageIndex = 0;
    tabIndex = 1;

    lcd.setCursor(0, lineIndex);
    lcd.write((byte)0);
    
    lcd.setCursor(2, 0);
    lcd.print(firstTab);

    lcd.setCursor(2, 1);
    lcd.print(secondTab);

    scrollActive = true;
  }

  if (joyMoved == false && joyY > joyMaxThreshold && tabIndex < arrayLenght - 1) {
    joyMoved = true;

    lineIndex = !lineIndex;
    tabIndex++;

    if (lineIndex == 0) {   // next page
      pageIndex++;
      lcd.clear();

      if (tabIndex != arrayLenght) {
        firstTab = tabArray[tabIndex];
        secondTab = tabArray[tabIndex + 1];
      }
      else secondTab = "                ";

      lcd.setCursor(2, 0);
      lcd.print(firstTab);

      lcd.setCursor(2, 1);
      lcd.print(secondTab);
    }
  
    lcd.setCursor(0, !lineIndex);
    lcd.print(" ");
    lcd.setCursor(0, lineIndex);
    lcd.write((byte)0);
  }

  if (joyMoved == false && joyY < joyMinThreshold && tabIndex > 1) {
    joyMoved = true;

    lineIndex= !lineIndex;
    tabIndex--;

    if (lineIndex == 1) {
      pageIndex--;
      lcd.clear();

      if (tabIndex != 0) {
        firstTab = tabArray[tabIndex - 1];
        secondTab = tabArray[tabIndex];
      }

      lcd.setCursor(2, 0);
      lcd.print(firstTab);

      lcd.setCursor(2, 1);
      lcd.print(secondTab);
    }
  
    lcd.setCursor(0, !lineIndex);
    lcd.print(" ");
    lcd.setCursor(0, lineIndex);
    lcd.write((byte)0);
  }

  if (joyY >= joyMinThreshold && joyY <= joyMaxThreshold) {
    joyMoved = false;
  }  
}

void scrollText(const char* text[], unsigned int lineNumber) {
  static unsigned int pageIndex = 0;
  static unsigned int firstLineIndex = 0;    // the index of the first line on a page

  static char* firstLine = text[firstLineIndex];
  static char* secondLine = text[firstLineIndex + 1];


  if (scrollActive == false) {
    pageIndex = 0;
    firstLineIndex = 0;
    firstLine = text[firstLineIndex];
    secondLine = text[firstLineIndex + 1];

    lcd.setCursor(0, 0);
    lcd.print(firstLine);

    lcd.setCursor(0, 1);
    lcd.print(secondLine);

    scrollActive = true;
  }

  if (joyMoved == false && joyY > joyMaxThreshold && pageIndex < lineNumber / 2 - 1 + lineNumber % 2) {
    // pageIndex < lineNumber / 2 - 1 + lineNumber % 2      means the pageIndex cant be bigger than half of the numebr of lines, but if the total is odd, u need one more page
    lcd.clear();

    joyMoved = true;

    firstLineIndex += 2;
    pageIndex++;

    if (pageIndex > lineNumber / 2 && lineNumber % 2 == 1) {    // odd numebr of lines means one line on the last page
      secondLine = "                ";
    }
    else {
      firstLine = text[firstLineIndex];
      secondLine = text[firstLineIndex + 1];
    }

    lcd.setCursor(0, 0);
    lcd.print(firstLine);

    lcd.setCursor(0, 1);
    lcd.print(secondLine);
  }

  if (joyMoved == false && joyY < joyMinThreshold && pageIndex > 0) {
    lcd.clear();

    joyMoved = true;

    firstLineIndex -= 2;
    pageIndex--;

    firstLine = text[firstLineIndex];
    secondLine = text[firstLineIndex + 1];

    lcd.setCursor(0, 0);
    lcd.print(firstLine);

    lcd.setCursor(0, 1);
    lcd.print(secondLine);    
  }

  if (joyY >= joyMinThreshold && joyY <= joyMaxThreshold) {
    joyMoved = false;
  }
}

#endif