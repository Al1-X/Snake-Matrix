#ifndef SNAKE_H
#define SNAKE_H

#include "LedControl.h"
#include "Variables.h"
#include "MatrixMiniGame.h"
#include <Arduino.h>

const int buzzerPin = 9;

const int dinPin = 12;
const int clockPin = 11;
const int loadPin = 10;

const int xPin = A0; 
const int yPin = A1;
const int switchPin = 2;

const int joyMinThreshold = 200;
const int joyMaxThreshold = 600;

unsigned long lastBlinkTime = 0;
int blinkDuration = 500;

const byte matrixSize = 8;

int xValue;
int yValue;

bool soundOn = true;

unsigned int score = 0;

LedControl ledControl = LedControl(dinPin, clockPin, loadPin, 1);

struct Point {
  int row = 0, col = 0;

  Point(int row = 0, int col = 0): row(row), col(col) {}
};

// constants for directions
const byte up = 1;
const byte right = 2;
const byte down = 3;
const byte left = 4;

int matrix[matrixSize][matrixSize] = {};

byte initialSnakeDirection = left;
byte snakeDirection = initialSnakeDirection;
float snakeSpeed = 0.0;    // the snake speed is actually a debounce that variates
bool foodActive = false;

bool win = false;
bool gameOver = false;

// adjustable variables
int initialSnakeLength = 3;
int snakeLength = initialSnakeLength;
float speedFactor = 10.0;

const int initialSnakeRow = 5;
const int initialSnakeCol = 7;

Point snakeHead(initialSnakeRow, initialSnakeCol);
Point food;

unsigned long lastMove =0;
int moveDuration = 300;

void blink() {
  if (millis() - lastBlinkTime > blinkDuration) {
    lastBlinkTime = millis();

    ledControl.setLed(0, food.row, food.col, !matrix[food.row][food.col]);
  }
}

void generateFood() {
    if(snakeLength == 64) {
      win = true;
    }
    do {
			food.row = random(0, matrixSize);
			food.col = random(0, matrixSize);
		} while (matrix[food.row][food.col] > 0);
    
    foodActive = true;
  }

unsigned long lastJoyMove = 0;
unsigned long joyDebounce = 10;
byte lastSnakeDirection = snakeDirection;

unsigned long lastSpeedChange = 0;
int speedChangeDuration = 3000;

float maxSpeed = 200.0;

void changeSpeed() {
  if (millis() - lastSpeedChange > speedChangeDuration) {
    lastSpeedChange = millis();

    if (snakeSpeed <= maxSpeed - speedFactor) {

    snakeSpeed += speedFactor;
    }
  }
}

void changeDirection() {
  unsigned long debouncePadding = millis();

  if(millis() - lastJoyMove > joyDebounce) {
    lastJoyMove = millis();

    xValue = analogRead(xPin);
    yValue = analogRead(yPin);

    if (xValue < joyMinThreshold) {
      snakeDirection = left;
    }
    else {
      if (xValue > joyMaxThreshold) {
        snakeDirection = right;
      }
    }

    if (yValue < joyMinThreshold) {
      snakeDirection = up;
    }
    else {
      if (yValue > joyMaxThreshold) {
        snakeDirection = down;
      }
    }

  }

}

void renderSnake() {
  if (snakeDirection == lastSnakeDirection - 2 ||
        snakeDirection == lastSnakeDirection + 2) {
      snakeDirection = lastSnakeDirection;
  }
  
  switch (snakeDirection) {
    case up:
      snakeHead.row--;
      if (snakeHead.row < 0)  snakeHead.row = 7;
      ledControl.setLed(0, snakeHead.row, snakeHead.col, HIGH);
      break;

    case down:
      snakeHead.row++;
      if (snakeHead.row > 7)  snakeHead.row = 0;
      ledControl.setLed(0, snakeHead.row, snakeHead.col, HIGH);
      break;

    case right:
      snakeHead.col++;
      if (snakeHead.col > 7)  snakeHead.col = 0;
      ledControl.setLed(0, snakeHead.row, snakeHead.col, HIGH);
      break;

    case left:
      snakeHead.col--;
      if (snakeHead.col < 0)  snakeHead.col = 7;
      ledControl.setLed(0, snakeHead.row, snakeHead.col, HIGH);
      break;                  
  }

  lastSnakeDirection = snakeDirection;

  if (matrix[snakeHead.row][snakeHead.col] > 1) {
    Serial.println("bro---------");
    gameOver = true;

    return;
  }

  if (snakeHead.row == food.row && snakeHead.col == food.col) {
    ledControl.setLed(0, food.row, food.col, HIGH);

    if (soundOn == true) {
      tone(buzzerPin, 1000, 250);
    }
    foodActive = false;

    snakeLength++;

		for (int row = 0; row < matrixSize; row++) {
			for (int col = 0; col < matrixSize; col++) {
				if (matrix[row][col] > 0) {
					matrix[row][col]++;
				}
			}
		}    
  }

  matrix[snakeHead.row][snakeHead.col] = snakeLength + 1; // will be decremented in a moment

	for (int row = 0; row < matrixSize; row++) {
		for (int col = 0; col < matrixSize; col++) {
			if (matrix[row][col] > 0) {
				matrix[row][col]--;
			}
			  ledControl.setLed(0, row, col, matrix[row][col] == 0 ? 0 : 1);
		}
	}
  blink();

  score = snakeLength - initialSnakeLength;
}

void playGameOverSound() {
  tone(buzzerPin, 1000, 200);
  delay(200);
  tone(buzzerPin, 800, 200);
  delay(200);
  tone(buzzerPin, 600, 200);
}

void playVictorySound() {
  tone(buzzerPin, 500, 200);
  delay(200);
  tone(buzzerPin, 500, 200);
  delay(200);
  tone(buzzerPin, 1000, 500);
}

void killSnake() {
	ledControl.setLed(0, food.row, food.col, 0);

	for (int i = snakeLength; i >= 1; i--) {
		for (int row = 0; row < 8; row++) {
			for (int col = 0; col < 8; col++) {
				if (matrix[row][col] == i) {
					ledControl.setLed(0, row, col, 0);
					delay(100);
				}
			}
		}
	}

}

int gameState = 1;

void endGame() {

  Serial.println("entered endgame");

  if (gameOver) {
    if (soundOn == true) {
    playGameOverSound(); 

    }
    killSnake();
    Serial.println("ko!");
  }

  if (win) {
    if (soundOn == true) {

    playVictorySound();
    }
    killSnake();
    Serial.println("i1n!");
  }

  gameState = 0;

}

void setupSnake() {
  gameState = 1;

  win = false;
	gameOver = false;
  snakeLength = initialSnakeLength;
  snakeDirection = initialSnakeDirection;
  lastSnakeDirection = snakeDirection;
  snakeSpeed = 0.0;

  snakeHead.row = initialSnakeRow;
  snakeHead.col = initialSnakeCol;

		for (int row = 0; row < 8; row++) {
			for (int col = 0; col < 8; col++) {
  matrix[row][col] = 0; 
      }
    }  

  ledControl.shutdown(0, false);
  ledControl.clearDisplay(0);

  randomSeed(analogRead(0));

  matrix[snakeHead.row][snakeHead.col] = snakeLength;

  Serial.begin(9600);
}


void playSnake() {

  if (gameOver == false && win == false) {
    if (foodActive == false) {

      generateFood();
    }
  changeDirection();
  changeSpeed();
    if (millis() - lastMove > moveDuration - snakeSpeed) {
      lastMove = millis();    
      renderSnake();
    }
  }

  else {
    endGame();
  };
}


#endif