/*
ATmega16:
16kb flash
1kb ram
512 kb eeprom
*/

#include "LedControl.h"
#include <EEPROM.h>
#include "LCD.h"
#include "Scroll.h"
#include "Player.h"
#include "Variables.h"
#include "Snake.h"

using namespace std;

Player* player = new Player();
int playerPosition;

const int lcdBrightnessAddress = 29;  // on eeprom
byte lcdBrightness;

const int mtrxBrigtnessAddress = 30;
byte matrixBrightness;

const int soundOnAddress = 31;

bool placedInLeaderboard;

void setup() {
  Serial.begin(9600);

  pinMode(blackLightPin, OUTPUT);
  pinMode(switchPin, INPUT_PULLUP);

  lcdBrightness = EEPROM.read(lcdBrightnessAddress);
  analogWrite(blackLightPin, lcdBrightness);

  matrixBrightness = EEPROM.read(mtrxBrigtnessAddress);

  soundOn = EEPROM.read(soundOnAddress);

  lcd.createChar(0, pointerArrow);
  lcd.createChar(1, snakeHeadByte);
  lcd.createChar(2, snakeBody1);
  lcd.createChar(3, snakeBody2);
  lcd.createChar(4, snakeTail);
  lcd.createChar(5, pointerArrowVertical);

  lcd.begin(16, 2);

  printText(0, 0, " Welcome player ");
  delay(500);
  printText(7, 1, "to");
  delay(750);
  lcd.clear();

  printLongText(0, 0, title, 200);
  printLongText(0, 1, startAnimation, 100);

  player -> enterName();

  lcd.clear();

  printText(4, 0, "Welcome,");

  printText(1, 1, "Challenger ");
  printText(12, 1, player -> getName());


  delay(1500);

  // for all the spots where the top scores will be, write 0 for future comparison to current player -> initializer
  // for (int address = 0; address <= 4 * (topPlayerNumber - 1); address+= 4) {   // verifica linia asta
  //   writePlayerToEeprom(address, Player());
  // }

  readLeaderboardFromEeprom();
  lcd.clear();
}

void readLeaderboardFromEeprom() {
  Player* leaderboardPlayer = new Player();

  // when the game is activated, create leaderboard from eeprom
  for (int address = 0; address <= 4 * (topPlayerNumber - 1); address+= 4) {
    leaderboardPlayer =  readPlayerFromEeprom(address);
    leaderboard[address / 4] = *leaderboardPlayer;
  }

  delete leaderboardPlayer;
}

void loop() {
  analogWrite(blackLightPin, lcdBrightness);

  buttonState = digitalRead(switchPin);
  joyY = analogRead(yPin);

  // system state change
  if (buttonState != lastButtonState && lastButtonState == HIGH) {
    scrollActive = false;

    switch (systemState) {
      case 0:     // menu
        setupSnake();
        menuOption();
        lcd.clear();
        break;
      case 100:   // gameplay
        killSnake();

        printText(0, 0, "   Game over!   ");
        printText(4, 1, "\x01\x02\x03\x02\x03\x02\x03\x04");

        delay(1000);

        placedInLeaderboard = placeInLeaderboard(player);
        for (int i = 0; i < topPlayerNumber; i++) {
          Serial.print(leaderboard[i].getName());
          Serial.print(" ");
          Serial.println(leaderboard[i].getScore());
        }
        lcd.clear();
        systemState = 200;
        break;
      case 200:   // End screen
        score = 0;      
        lcd.clear();
        systemState = 0;
        break;
      case 2:     // Settings
        settingsMenu();
        lcd.clear();
        break;
      case 3:     // Leaderboard
        lcd.clear();
        systemState = 0;
        break;      
      case 4:     // How to play
        lcd.clear();
        systemState = 0;
        break;
      case 5:     // About
        lcd.clear();
        systemState = 0;
        break;
      case 6:     // Modify Name
        lcd.clear();
        systemState = 2;
        break;
      case 7:    // Difficulty
        lcd.clear();
        systemState = 2;
        break;
      case 8:    // LCD Brightness
        EEPROM.update(lcdBrightnessAddress, lcdBrightness);
        lcd.clear();
        systemState = 2;
        break;
      case 9:    // Matrix Brightness
        EEPROM.update(mtrxBrigtnessAddress, matrixBrightness);
        for (int row = 0; row < matrixSize; row++) {
          ledControl.setRow(0, row, B00000000);
        }    
        ledControl.shutdown(0, true);
        lcd.clear();
        systemState = 2;
        break;
      case 10:    // Sounds
        EEPROM.update(soundOnAddress, soundOn);        
        lcd.clear();
        systemState = 2;
        break;
    }
  }


  // systeam state actions
  switch (systemState) {
    case 0:     // MENU
      if (millis() - wheelJoyLastMove > wheelDebounceDuration) {
        wheelJoyLastMove = millis();
        scrollWheel(menuTabs, menuLenght);
      }
      break;    // GAMEPLAY
    case 100:
      playSnake(); 
      if (gameState == 0) {
      Serial.println("game ended");
        systemState = 200;
      }
      lcd.setCursor(0, 0);
      lcd.print("Score:");
      lcd.setCursor(7, 0);
      lcd.print(score);
      delay(150);
      break;
    case 200:   // END SCREEN
      lcd.setCursor(0, 0);
      lcd.print(" U scored " + String(score) + " pts");
      lcd.setCursor(0, 1);
      if (placedInLeaderboard == true) {
        lcd.print(" Ur rank is " + String(playerPosition + 1) + "!!");
      }
      else {
        lcd.print("   No rank :(   ");
      }
      break;
    case 2:     // SETTINGS
      if (millis() - wheelJoyLastMove > wheelDebounceDuration) {
        wheelJoyLastMove = millis();
        scrollWheel(settings, settingsLenght);
      }  
    case 3:     // LEADERBOARD
      if (millis() - wheelJoyLastMove > wheelDebounceDuration) {
        wheelJoyLastMove = millis();
        scrollText(leaderboardString, topPlayerNumber + 1);
      }
      break;      
    case 4:     // HOW TO PLAY
      if (millis() - wheelJoyLastMove > wheelDebounceDuration) {
        wheelJoyLastMove = millis();    
        scrollText(howToPlay, howToPlayLenght);
      }
      break;
    case 5:     // ABOUT
      if (millis() - wheelJoyLastMove > wheelDebounceDuration) {
        wheelJoyLastMove = millis();
        scrollText(about, aboutLenght);
      }
      break;
    case 6:     // MODIFY NAME
      player -> enterName();
      break;
    case 7:     // DIFICULTY
      printText(3, 0, "Difficulty");
      changeDifficulty();
      break;
    case 8:     // LCD BRIGHTNESS
      printText(1, 0, "LCD Brightness");
      changeLcdBrightness();
      break;
    case 9:     // MATRIX BRIGHTNESS
      printText(0, 0, "Mtrx Brightness");
      changeMatrixBrightness();
      break;
    case 10:     // SOUNDS
      printText(5, 0, "Sounds");
      changeSound();
      break;     
  }

  lastButtonState = buttonState;
}

void changeSound() {
  static unsigned long lastSwitchMove = 0;
  int switchDebounceDuration = 300;
  int xValue;

  xValue = analogRead(xPin);

  if (millis() - lastSwitchMove > switchDebounceDuration) {
    lastSwitchMove = millis();

    if (xValue < joyMinThreshold || xValue > joyMaxThreshold) {
      soundOn = !soundOn;
    }    
  }

  switch (soundOn) {
    case 1:     // SOUNDS ON
      printText(0, 1, "       ON       ");      
      break;
    case 0:     // SOUNDS OFF
      printText(0, 1, "       OFF       ");      
      break;
  }
}

void changeMatrixBrightness() {
  static unsigned long lastSwitchMove = 0;
  int switchDebounceDuration = 300;
  int xValue;

  ledControl.shutdown(0, false);
  for (int row = 0; row < matrixSize; row++) {
    ledControl.setRow(0, row, B11111111);
  }

  xValue = analogRead(xPin);

  if (millis() - lastSwitchMove > switchDebounceDuration) {
    lastSwitchMove = millis();

    if (xValue < joyMinThreshold) {
      if (matrixBrightness == 1) {
        matrixBrightness = 3;
      }
      else {
        matrixBrightness--;
      }
    }

    if (xValue > joyMaxThreshold) {

      if (matrixBrightness == 3) {
        matrixBrightness = 1;
      }
      else {
        matrixBrightness++;
      }
    }
    
  }

  switch (matrixBrightness) {
    case 1:     // LOW    
      printText(0, 1, "      ~LOW      ");
      break;
    case 2:     // NORMAL
      printText(0, 1 , "     NORMAL     ");
      break;
    case 3:     // HIGH
      printText(0, 1, "      HIGH      ");
      break;
  }

  ledControl.setIntensity(0, matrixBrightness);

  Serial.println(matrixBrightness);
}

void changeLcdBrightness() {
  static byte brightness = lcdBrightness / 50;
  static unsigned long lastSwitchMove = 0;
  int switchDebounceDuration = 300;
  int xValue;

  xValue = analogRead(xPin);
  // Serial.println(xValue);

  if (millis() - lastSwitchMove > switchDebounceDuration) {
    lastSwitchMove = millis();

    if (xValue < joyMinThreshold) {
      if (brightness == 1) {
        brightness = 5;
      }
      else {
        brightness -= 2;
      }
    }

    if (xValue > joyMaxThreshold) {

      if (brightness == 5) {
        brightness = 1;
      }
      else {
        brightness += 2;
      }
    }
    
  }

  switch (brightness) {
    case 1:     // LOW    
      printText(0, 1, "      ~LOW      ");
      break;
    case 3:     // NORMAL
      printText(0, 1 , "     NORMAL     ");
      break;
    case 5:     // HIGH
      printText(0, 1, "      HIGH      ");
      break;
  }

  lcdBrightness = brightness * 50;
  analogWrite(blackLightPin, lcdBrightness);

  Serial.println(lcdBrightness);
}

void changeDifficulty() {
  static byte difficulty = 3;
  static unsigned long lastSwitchMove = 0;
  int switchDebounceDuration = 500;
  int xValue;

  xValue = analogRead(xPin);

  if (millis() - lastSwitchMove > switchDebounceDuration) {
    lastSwitchMove = millis();

    if (xValue < joyMinThreshold) {

      if (difficulty == 3) {
        difficulty = 1;
      }
      else {
        difficulty++;
      }
    }

    if (xValue > joyMaxThreshold) {

      if (difficulty == 1) {
        difficulty = 3;
      }
      else {
        difficulty--;
      }
    }
    
  }

  switch (difficulty) {
    case 3:     // easy
      printText(0, 1, "      Easy      ");
      break;
    case 2:     // medium
      printText(0, 1 , "     Medium     ");
      break;
    case 1:     // extreme
      printText(0, 1, "    Extreme!    ");
      break;
  }

  speedChangeDuration = difficulty * 1000;
}

void settingsMenu() {
  switch (tabIndex) {
    case 1:     // Modify Name
      systemState = 6;
      break;
    case 2:     // Difficulty
      systemState = 7;
      break;
    case 3:     // LCD Brightness
      systemState = 8;
      break;
    case 4:     // Matrix brightness
      systemState = 9;
      break;
    case 5:     // Sounds
      systemState = 10;
      break;    
    case 6:     // Back to Menu
      systemState = 0;
      break;
  }
}

// main menu option change
void menuOption() {
  switch (tabIndex) {
    case 1:               // Start journey!
      systemState = 100;
      break;
    case 2:               // Settings
      systemState = 2;
      break;
    case 3:               // Leaderboard
      for (int i = 0; i < topPlayerNumber; i++) {
        Serial.print(leaderboard[i].getName());
        Serial.print(" ");
        Serial.println(leaderboard[i].getScore());
      }

      for (int i = 1; i <= topPlayerNumber; i++) {
        leaderboardString[i] = "";
        toString(leaderboardString[i], &leaderboard[i - 1], String(i));
      }
      systemState = 3;
      break;
    case 4:               // How to play
      systemState = 4;
      break;
    case 5:
      systemState = 5;    // About
      break;
  }
}

void writePlayerToEeprom(int address, Player player)
{
  char* playerName = player.getName();
    Serial.println(playerName);

  for (int i = 0; i < 3; i++) {
    EEPROM.update(address + i, playerName[i]);
  }

  EEPROM.update(address + 3, player.getScore());
}

Player* readPlayerFromEeprom(int address)
{
  char playerName[3];
  unsigned int playerScore;
  
  for (int i = 0; i < 3; i++) {
    playerName[i] = EEPROM.read(i);
  }
  playerName[3] = '\0';

  playerScore = EEPROM.read(address + 3);

  Player* player = new Player(playerName, playerScore);

  return player;
}

bool placeInLeaderboard(Player* player)
{
  // check where the current score fits and replace add to array, moving everyone one step back
  bool placedInLeaderboard = false;

  for (playerPosition = 0; playerPosition < topPlayerNumber; playerPosition++) {
    if (score > leaderboard[playerPosition].getScore()) {
      break;
    }
  }

  while (score == leaderboard[playerPosition - 1].getScore() && strcmp(player -> getName(), leaderboard[playerPosition - 1].getName())) {
    playerPosition--;
  }

  if (player -> getHighestRank() > playerPosition + 1) {
    player -> setHighestRank(playerPosition + 1);
  }
  
  for (int i = topPlayerNumber - 1; i > playerPosition; i--) {
    leaderboard[i] = leaderboard[i - 1];
  }

  leaderboard[playerPosition] = *player;

  if (playerPosition <= topPlayerNumber) {
    placedInLeaderboard = true;
  }

  return placedInLeaderboard;
}
