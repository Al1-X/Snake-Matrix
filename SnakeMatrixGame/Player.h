#ifndef PLAYER_H
#define PLAYER_H

#include "LCD.h"
#include "Snake.h"
#include <Arduino.h>

using namespace std;

byte pointerArrowVertical[] = {
  B00000,
  B00000,
  B00100,
  B01110,
  B11111,
  B00000,
  B00000,
  B00000
};

class Player {
  //parameters
  char* name;
  unsigned int score;
  unsigned int highestRank = 0;

  // methods
public:
  Player() : name("AAA"), score(0) {};
  Player(char* Name, unsigned int Score): name(Name), score(Score) {};

  char* getName() { return name; };

  unsigned int getScore() { return score; };

  unsigned int getHighestRank() { return highestRank; };

  void setName(char* Name) { name = Name; };

  void setHighestRank(unsigned int Score) { score = Score; };

  void setRank(unsigned int Rank) { highestRank = Rank; };

/*
- using the joystick u can move and change letters
- on click the name is saved and the player is prompted to the main menu
*/
  void enterName() {
    // enter name with joystick -> 3 LETTERS

    lcd.setCursor(0, 0);
    lcd.print("My name is : ");

    byte switchState = HIGH;
    unsigned int arrowPosition = 13;   // can be 13 14 15
    unsigned int lastArrowPosition;
    unsigned long lastSwitchMove = 0;
    int switchDebounceDuration = 300;
    int xValue;
    int yValue;

    bool movedOnX = false;    // stops movement on both axis
    byte letterCode = 65;   // starts with A

    while (switchState == HIGH) {
      switchState = digitalRead(switchPin);
      
      xValue = analogRead(xPin);
      yValue = analogRead(yPin);

      lastArrowPosition = arrowPosition;

      if (millis() - lastSwitchMove > switchDebounceDuration) {
        lastSwitchMove = millis();

        if (xValue > joyMaxThreshold) {
          if (arrowPosition == 15) {
            arrowPosition = 13;
          }

          else {
            arrowPosition++;
          }

          movedOnX = true;
        }

        if (xValue < joyMinThreshold) {
          if (arrowPosition == 13) {
            arrowPosition = 15;
          }

          else {
            arrowPosition--;
          }

          movedOnX = true;
        }

        letterCode = int(name[arrowPosition - 13]);

        if (movedOnX == false) {
          if (yValue > joyMaxThreshold) {
            if (letterCode == 65) {
              letterCode = 90;
            }

            else {
              letterCode--;
            }
          }

          if (yValue < joyMinThreshold) {
            if (letterCode == 90) {
              letterCode = 65;
            }

            else {
              letterCode++;
            }
          }

          name[arrowPosition - 13] = char(letterCode);
        }

        if (movedOnX == true) {
          lcd.setCursor(lastArrowPosition, 1);
          lcd.print(" ");
        }
        
        movedOnX = false;

        lcd.setCursor(arrowPosition, 1);
        lcd.write(byte(5));

        lcd.setCursor(13, 0);
        lcd.print(name);
      }
    }
  }
};

char* toString(char* &string, Player* player, String rank) {
    char rankString[2];

    char scoreString[3];

    strcpy(rankString, rank.c_str());
    sprintf(scoreString, "%u", player -> getScore());
    scoreString[3] = '\0';

    strcpy(string, " ");
    strcat(string, rankString);
    strcat(string, ".  ");
    strcat(string, player -> getName());
    strcat(string, "    ");
    strcat(string, scoreString);
    strcat(string, "\0");
}

#endif