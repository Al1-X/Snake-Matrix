#ifndef VARIABLES_H
#define VARIABLES_H

#include "Player.h"

byte pointerArrow[] = {
  B00000,
  B01000,
  B01100,
  B01110,
  B01110,
  B01100,
  B01000,
  B00000
};

byte snakeHeadByte[] = {
  B00000,
  B00110,
  B01011,
  B11111,
  B11111,
  B01011,
  B00110,
  B00000
};

byte snakeBody1[] = {
  B00100,
  B01110,
  B10111,
  B11110,
  B01011,
  B10001,
  B00000,
  B00000
};

byte snakeBody2[] = {
  B00000,
  B00000,
  B10001,
  B01011,
  B11101,
  B11011,
  B01110,
  B00100
};

byte snakeTail[] = {
  B00000,
  B00000,
  B10000,
  B11101,
  B11110,
  B10000,
  B00000,
  B00000
};

const byte settingsCog[matrixSize] = {
  B01011010,
  B11111111,
  B01100110,
  B11000011,
  B11000011,
  B01100110,
  B11111111,
  B01011010
};

const byte menuSymbol[matrixSize] = {
  B11000011,
  B11100111,
  B11100111,
  B11111111,
  B11011011,
  B11000011,
  B11000011,
  B11000011
};

bool actionStarted = false;
unsigned long elapsedTime = 0;
byte buttonState;
byte lastButtonState = HIGH;
unsigned long wheelDebounceDuration = 60;
unsigned long wheelJoyLastMove = 0;

unsigned int tabIndex = 1;

int joyY;

char * title = "                SNAAAKE 2.022                ";
char* startAnimation = "                \x01\x03\x02\x03\x02\x03\x04                ";

int systemState = 0;

byte menuState = 0;

const unsigned int menuLenght = 6;
const char* menuTabs[menuLenght + 1] = {"    Menu    ", "Start game!", "Settings", "Leaderboard", "How to play", "About"};
//                                            0               1               2             3              4           5

const unsigned int howToPlayLenght = 10;
const char* howToPlay[howToPlayLenght + 1] = {"Use the joystick", "to move.", "Press the button", "to end game!", "Collect food", "for extra points.", "Avoid eatting", "your tail!", "   Good luck,   ", "    player !    "};

const unsigned int aboutLenght = 11;
const char* about[aboutLenght + 1] = {"Designed by", "Alina Georgescu.", "This game was", "created for the", "Introduction to", "Robotics course", "from 2022-2023", "taken at FMI.", "Find me @AlinaGeo", "on github", "Thx 4 playing <3"};

const unsigned int settingsLenght = 7;
const char* settings[settingsLenght + 1] = {"  Settings  ", "Modify name", "Difficulty", "LCD brightness", "Mtrx brightness", "Sounds", "Back to Menu"};

const byte topPlayerNumber = 7;
Player leaderboard[topPlayerNumber];

char* leaderboardString[topPlayerNumber + 1] = {"Rank Name  Score"};

#endif