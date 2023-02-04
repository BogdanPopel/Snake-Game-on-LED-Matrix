#include <LiquidCrystal.h>
#include <LedControl.h>
#include <EEPROM.h>
#include "song.h"
#include "symbols.h"

#define INTRODUCTION 0
#define MAIN_MENU 1
#define ABOUT 2
#define HOW_TO_PLAY 3
#define SETTINGS 4
#define RESET_SCORE 5
#define LEADERBOARD 6
#define START_GAME 7
byte state = INTRODUCTION;

const byte dinPin = 12;
const byte clockPin = 11;
const byte loadPin = 10;
LedControl lc = LedControl(dinPin, clockPin, loadPin, 1);
const byte rs = 9;
const byte enable = 8;
const byte d4 = 7;
const byte d5 = 6;
const byte d6 = 3;
const byte d7 = 4;
const byte backLight = 5;

const int pinSW = 2;
const int pinX = A0;
const int pinY = A1;
const byte buzzerPin = A2;

byte buttonValue = LOW;
int xValue = 0;
int yValue = 0;
bool joyMoved = false;
bool brightnessJoyMoved = false;
bool modifyLetterJoyMoved = false;
const int minThreshold = 400;
const int maxThreshold = 600;
const int mainMenuItems = 6;
const int settingsItems = 3;
const int levelItems = 3;
const int leaderboardSize = 6;
int currentMenuItem = 0;
int currentSettingsItem = 0;
int currentLevelDisplay = 0;
int currentRankItem = 0;
String mainMenuElements[mainMenuItems] = { "1. Start Game", "2. Leaderboard", "3. Settings", "4. About", "5. How To Play", "6. Reset Score" };
String settingsElements[settingsItems] = { "LCD Light:", "Matrix Light:", "Sound:" };
String levels[levelItems] = { " EASY ", "MEDIUM", " HARD " };
String leaderboardPlaces[leaderboardSize] = { "1st.", "2nd.", "3rd.", "4th.", "5th.", "6th" };
bool displayMainMenuPosition = false;
String newPlayer = "";
int newScore = 0;


char* introText = "Snake, brought to you by Nokia to enjoy! :)                ";
char* aboutText = "Homework for IntroductionToRobotics by Popel Bogdan -> Snake Game               ";
char* howToPlayText = "If you haven't heard of snake by now, there is no saving for you. Go play another game!                  ";
const int defaultTextCursor = 0;
int scrollingTextCursor = 0;

int buttonStatePrevious = LOW;
unsigned long minButtonLongPressDuration = 3000;
unsigned long buttonLongPressMillis;
bool buttonLongPressed = false;
const int intervalButton = 200;
unsigned long previousButtonMillis;
unsigned long buttonPressDuration;
unsigned long currentMillis;
unsigned long previousTime = 0;
const int scrollTextTime = 400;

const int alphabetLetters = 26;
int currentModifiedLetter = 0;
const int playerNameLength = 5;
int enterNameLetters[playerNameLength] = { 0, 0, 0, 0, 0 };
char alphabet[26] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ_";
const int enterNameLeftLimitCursor = 10;
const int enterNameRightLimitCursor = 14;
int enterNameArrowPosition = enterNameLeftLimitCursor;
int menuPosition = 1;
bool enterUserNameDisplay = true;
bool enterLevelDisplay = false;
volatile byte lcdBrightness = 5;
const int lcdBrightnessIncrement = 50;
int matrixBrightness = 5;
const int matrixBrightnessStep = 3;
bool sound = false;
bool resetScoreFlag = false;
bool gameDisplay = false;

int playerScore;
String playerName;
const int eepromLcdBrightness = 1;
const int eepromMatrixBrightness = 2;
const int eepromSound = 3;
const int eepromScoreStart = 4;
const int eepromScoreEnd = 9;
const int eepromNameStep = 5;
const int eepromNameStart = 10;
int letterCounter = 0;
const int eepromNameEnd = 39;
int previousScore;
int scoreToMap;
int letterToMap;

int randomCol;
int randomRow;
unsigned long prevTimeGame = 0;
bool shouldGenerateFood = true;
int dotRowPosition = 0;
int dotColPosition = 0;
const int blinkTime = 150;
bool ledPower = true;
int deleteDotPosition = 0;
const int matrixDotNumber = 45;
int rowSnakePosition[matrixDotNumber] = { 0 };
int colSnakePosition[matrixDotNumber] = { 0 };
bool up = false;
bool down = false;
bool right = false;
bool left = false;
bool endGame = false;
float easySpeed = 400.0;
float hardSpeed = 200.0;
unsigned long previousSnakeTime = 0;
bool easyLevel = false;
bool mediumLevel = false;
bool hardLevel = false;
const int matrixMessageSpeed = 15;

LiquidCrystal lcd(rs, enable, d4, d5, d6, d7);

void setup() {
  
  lcd.createChar(0, tickMark);
  lcd.createChar(1, rightArrow);
  lcd.write(byte(0));
  lc.shutdown(0, false);
  lc.clearDisplay(0);

  lcd.begin(16, 2);
  pinMode(backLight, OUTPUT);
  pinMode(pinSW, INPUT_PULLUP);

  sound = EEPROM.read(eepromSound);
  for (int i = 0; i < matrixSize; i++) {
        lc.setRow(0, i, matrixShapes[8][i]);
    }
  if(sound)
    song();

  Serial.begin(9600);
}

void loop() {
    switch(state) {
    case INTRODUCTION:
      lcd.setCursor(1, 0);
      lcd.print("Snake Game");
      scrollingMessage(introText, 1);
      currentMillis = millis();

      // skip intro
      if (currentMillis - previousButtonMillis > intervalButton) {
        buttonValue = digitalRead(pinSW);
        buttonValue = !buttonValue;
        if (buttonValue == HIGH) {
          if (sound) {
            tone(buzzerPin, 1500, 5);
          }
          state = MAIN_MENU;
          lc.clearDisplay(0);
          lcd.clear();
        }
        previousButtonMillis = currentMillis;
      }

      break;
    case MAIN_MENU:
      displayMenu(state);
      break;
    case ABOUT:
      lcd.setCursor(5, 0);
      lcd.print("About");
      scrollingMessage(aboutText, 1);
      readControlMenu(state);
      break;
    case HOW_TO_PLAY:
      lcd.setCursor(2, 0);
      lcd.print("How to play");
      scrollingMessage(howToPlayText, 1);
      readControlMenu(state);
      break;
    case SETTINGS:
      settingsMenu(state);
      break;
    case RESET_SCORE:
      resetScoreMenu(state);
      break;
    case LEADERBOARD:
      leaderboardMenu(state);
      break;
    case START_GAME:
      startGameMenu(state);
    break;          
  }
  
  playerName = "";
  for (int i = eepromNameStart + (eepromNameStep * currentRankItem); i < eepromNameStep + (eepromNameStart + (eepromNameStep * currentRankItem)); i++) {
    playerName.concat(char(EEPROM.read(i)));
  }
  playerScore = EEPROM.read(eepromScoreStart + currentRankItem);
  lcdBrightness = EEPROM.read(eepromLcdBrightness);
  matrixBrightness = EEPROM.read(eepromMatrixBrightness);
  sound = EEPROM.read(eepromSound);

  analogWrite(backLight, (lcdBrightness * lcdBrightnessIncrement));
  lc.setIntensity(0, (matrixBrightness * matrixBrightnessStep));
}

// code-game
void snake() {

  lcd.setCursor(0, 0);
  lcd.print("Score:");
  lcd.setCursor(7, 0);
  lcd.print(newScore);
  lcd.setCursor(0, 1);
  lcd.print("Speed:");
  lcd.setCursor(6, 1);
  

  generateFood();
  snakeDirection();
  if (easyLevel || mediumLevel) {
    speed(easySpeed);
    lcd.print(100.0/easySpeed);
  } else if (hardLevel) {
    lcd.print(100.0/hardSpeed);
    speed(hardSpeed);
  }
  // update points
  if (dotRowPosition == randomRow && dotColPosition == randomCol) {
    if (sound) {
      tone(buzzerPin, 6500, 20);
    }
    newScore++;
    shouldGenerateFood = true;
    deleteDotPosition++;
  }
  returnToMenu();
}

void speed(float& speed) {
  if (millis() - previousSnakeTime >= speed) {
   
    if (up) {
      deleteTail();
      if (dotRowPosition > 0) {
        dotRowPosition--;
      } else {
        dotRowPosition = matrixSize - 1;
      }
      rowSnakePosition[0] = dotRowPosition;
    } else if (down) {
      deleteTail();
      if (dotRowPosition < matrixSize - 1) {
        dotRowPosition++;
      } else {
        dotRowPosition = 0;
      }
      rowSnakePosition[0] = dotRowPosition;
    } else if (right) {
      deleteTail();
      if (dotColPosition < matrixSize - 1) {
        dotColPosition++;
      } else {
        dotColPosition = 0;
      }
      colSnakePosition[0] = dotColPosition;
    } else if (left) {
      deleteTail();
      if (dotColPosition > 0) {
        dotColPosition--;
      } else {
        dotColPosition = matrixSize - 1;
      }
      colSnakePosition[0] = dotColPosition;
    }
    
    speed -= 0.5;
    previousSnakeTime = millis();
  }
}


void generateFood() {
  if (shouldGenerateFood) {
    if (easyLevel) {
      randomRow = random(0, 8);
      randomCol = random(0, 8);
    } else 
        if (mediumLevel || hardLevel) {
      randomRow = random(1, 7);
      randomCol = random(1, 7);
    }
    shouldGenerateFood = false;
  }
  for (int i = 0; i <= deleteDotPosition; i++) { //Delete dot position is also the size of the snake
    if (randomRow == rowSnakePosition[i] && randomCol == colSnakePosition[i]) {
      shouldGenerateFood = true;
    } else { //blink food
      if (millis() - previousTime >= blinkTime) {
        lc.setLed(0, randomRow, randomCol, ledPower);
        if (ledPower) {
          ledPower = false;
        } else {
          ledPower = true;
        }
        previousTime = millis();
      }
    }
  }
}


void snakeDirection() {
  xValue = analogRead(pinX);
  yValue = analogRead(pinY);

  if (mediumLevel || hardLevel) {
    for (int i = 0; i < matrixSize; i++) {
      lc.setLed(0, 0, i, true);
      lc.setLed(0, i, 0, true);
      lc.setLed(0, matrixSize - 1, i, true);
      lc.setLed(0, i, matrixSize - 1, true);
    }
  }

  // up
  if (yValue < minThreshold && joyMoved == false) {
    if (!down) {
      right = false;
      left = false;
      up = true;
    }

    if (sound) {
      tone(buzzerPin, 3500, 5);
    }
    joyMoved = true;
  }

  // down
  if (yValue > maxThreshold && joyMoved == false) {
    if (!up) {
      right = false;
      left = false;
      down = true;
    }

    if (sound) {
      tone(buzzerPin, 3500, 5);
    }
    joyMoved = true;
  }

  // left
  if (xValue < minThreshold && joyMoved == false) {
    if (!right) {
      up = false;
      down = false;
      left = true;
    }

    if (sound) {
      tone(buzzerPin, 3500, 5);
    }
    joyMoved = true;
  }

  // right
  if (xValue > maxThreshold && joyMoved == false) {
    if (!left) {
      up = false;
      down = false;
      right = true;
    }

    if (sound) {
      tone(buzzerPin, 3500, 5);
    }
    joyMoved = true;
  }

  if (yValue >= minThreshold && yValue <= maxThreshold && xValue >= minThreshold && xValue <= maxThreshold) {
    joyMoved = false;
  }

  lc.setLed(0, dotRowPosition, dotColPosition, true);
}


void deleteTail() {
  lc.setLed(0, rowSnakePosition[deleteDotPosition], colSnakePosition[deleteDotPosition], false);
  for (int i = deleteDotPosition; i > 0; i--) {
    rowSnakePosition[i] = rowSnakePosition[i - 1]; //make the tail go backwards in the array
    colSnakePosition[i] = colSnakePosition[i - 1];
  }
}


// code - menu
void displayMenu(byte& state) {

  yValue = analogRead(pinY);
  // matrix
  for (int i = 0; i < matrixSize; i++) {
    lc.setRow(0, i, matrixShapes[menuPosition-1][i]);
  }
  
  // menu position
  if (displayMainMenuPosition == false) {
    menuPosition = currentMenuItem + 1;
    lcd.setCursor(2, 1);
    lcd.write(byte(1));
  } 
  else {
    menuPosition = currentMenuItem + 2;
    if (currentMenuItem == mainMenuItems - 1) {
      menuPosition = 1;
    }
    lcd.setCursor(2, 0);
    lcd.write(byte(1));
  }

  lcd.setCursor(0, 1);
  lcd.print(mainMenuElements[currentMenuItem]);
  lcd.setCursor(0, 0);

  if (currentMenuItem == mainMenuItems - 1) {
    lcd.print(mainMenuElements[0]);
  }
  else {
    lcd.print(mainMenuElements[currentMenuItem + 1]);
  }

  // up
  if (yValue < minThreshold && joyMoved == false) {
    if (displayMainMenuPosition == false) {
      displayMainMenuPosition = true;
    } 
    else {
      if (currentMenuItem < mainMenuItems - 1) {
        currentMenuItem++;
      } 
      else {
        currentMenuItem = 0;
      }
      displayMainMenuPosition = false;
    }

    if (sound) {
      tone(buzzerPin, 3500, 5);
    }
    lcd.clear();
    lc.clearDisplay(0);
    joyMoved = true;
  }

  // down
  if (yValue > maxThreshold && joyMoved == false) {
    if (displayMainMenuPosition == true) {
      displayMainMenuPosition = false;
    } 
    else {
      if (currentMenuItem > 0) {
        currentMenuItem--;
      } 
      else {
        currentMenuItem = 4;
      }
      displayMainMenuPosition = true;
    }

    if (sound) {
      tone(buzzerPin, 3500, 5);
    }
    lcd.clear();
    lc.clearDisplay(0);
    joyMoved = true;
  }

  if (yValue >= minThreshold && yValue <= maxThreshold) {
    joyMoved = false;
  }
  readControlMenu(state);
}


// code-start
void startGameMenu(byte& state) {
  if (enterUserNameDisplay) {
    enterUserName();
  } 
  else 
    if (enterLevelDisplay) {
      enterLevel();
    } 
    else 
      if (gameDisplay) {
      snake();
    }
  readControlMenu(state);
}

void enterLevel() {
  xValue = analogRead(pinX);

  if (currentLevelDisplay == 0) {
    lcd.setCursor(15, 0);
    lcd.print(">");
    hardLevel = false;
    mediumLevel = false;
    easyLevel = true;
  } else 
    if (currentLevelDisplay == levelItems - 1) {
      lcd.setCursor(0, 0);
      lcd.print("<");
      mediumLevel = false;
      easyLevel = false;
      hardLevel = true;
    } 
    else {
    lcd.setCursor(0, 0);
    lcd.print("<");
    lcd.setCursor(15, 0);
    lcd.print(">");
    easyLevel = false;
    hardLevel = false;
    mediumLevel = true;
  }

  lcd.setCursor(3, 0);
  lcd.print("Difficulty");
  lcd.setCursor(5, 1);
  lcd.print(levels[currentLevelDisplay]);

  // left
  if (xValue < minThreshold && joyMoved == false) {
    if (currentLevelDisplay > 0) {
      currentLevelDisplay--;
    }
    if (sound) {
      tone(buzzerPin, 3500, 5);
    }
    lcd.clear();
    joyMoved = true;
  }

  // right
  if (xValue > maxThreshold && joyMoved == false) {
    if (currentLevelDisplay < levelItems - 1) {
      currentLevelDisplay++;
    }
    if (sound) {
      tone(buzzerPin, 3500, 5);
    }
    lcd.clear();
    joyMoved = true;
  }

  // neutral joystick
  if (xValue >= minThreshold && xValue <= maxThreshold) {
    joyMoved = false;
  }
}

void enterUserName() {
  lcd.setCursor(enterNameArrowPosition, 0);
  lcd.print("v");
  lcd.setCursor(0, 0);
  lcd.print("HOLD: ");
  lcd.write(byte(0));
  lcd.setCursor(0, 1);
  lcd.print("Name:");
  lcd.setCursor(9, 1);
  lcd.print(">");
  lcd.setCursor(15, 1);
  lcd.print("<");
  moveNameArrow();
  // print the name
  lcd.setCursor(enterNameLeftLimitCursor, 1);
  lcd.print(alphabet[enterNameLetters[0]]);
  lcd.setCursor(enterNameLeftLimitCursor + 1, 1);
  lcd.print(alphabet[enterNameLetters[1]]);
  lcd.setCursor(enterNameLeftLimitCursor + 2, 1);
  lcd.print(alphabet[enterNameLetters[2]]);
  lcd.setCursor(enterNameLeftLimitCursor + 3, 1);
  lcd.print(alphabet[enterNameLetters[3]]);
  lcd.setCursor(enterNameLeftLimitCursor + 4, 1);
  lcd.print(alphabet[enterNameLetters[4]]);
  
  pickLetters();
}

void pickLetters() {
  yValue = analogRead(pinY);

  // up
  if (yValue < minThreshold && modifyLetterJoyMoved == false) {
    if (enterNameLetters[currentModifiedLetter] < alphabetLetters) {
      enterNameLetters[currentModifiedLetter]++;
    }
    if (sound) {
      tone(buzzerPin, 3500, 5);
    }
    lcd.clear();
    modifyLetterJoyMoved = true;
  }

  // down
  if (yValue > maxThreshold && modifyLetterJoyMoved == false) {
    if (enterNameLetters[currentModifiedLetter] > 0) {
      enterNameLetters[currentModifiedLetter]--;
    }
    if (sound) {
      tone(buzzerPin, 3500, 5);
    }
    lcd.clear();
    modifyLetterJoyMoved = true;
  }

  if (yValue >= minThreshold && yValue <= maxThreshold) {
    modifyLetterJoyMoved = false;
  }
}


void moveNameArrow() {
  xValue = analogRead(pinX);

  // right
  if (xValue < minThreshold && joyMoved == false) {
    if (enterNameArrowPosition > enterNameLeftLimitCursor) {
      currentModifiedLetter--;
      enterNameArrowPosition--;
    }
    if (sound) {
      tone(buzzerPin, 3500, 5);
    }
    lcd.clear();
    joyMoved = true;
  }

  // left
  if (xValue > maxThreshold && joyMoved == false) {
    if (enterNameArrowPosition < enterNameRightLimitCursor) {
      currentModifiedLetter++;
      enterNameArrowPosition++;
    }
    if (sound) {
      tone(buzzerPin, 3500, 5);
    }
    lcd.clear();
    joyMoved = true;
  }

  // neutral joystick
  if (xValue >= minThreshold && xValue <= maxThreshold) {
    joyMoved = false;
  }
}


// code - settings
void settingsMenu(byte& state) {
  xValue = analogRead(pinX);

  if (currentSettingsItem == 0) {
    lcd.setCursor(15, 0);
    lcd.print(">");
  } else if (currentSettingsItem == settingsItems - 1) {
    lcd.setCursor(0, 0);
    lcd.print("<");
  } else {
    lcd.setCursor(0, 0);
    lcd.print("<");
    lcd.setCursor(15, 0);
    lcd.print(">");
  }

  if (currentSettingsItem == 0) {
    lcd.setCursor(13, 1);
    lcd.print(EEPROM.read(eepromLcdBrightness));
    lcd.setCursor(14, 1);
    lcd.print("/5");
  } else if (currentSettingsItem == 1) {
    lcd.setCursor(13, 1);
    lcd.print(EEPROM.read(eepromMatrixBrightness));
    lcd.setCursor(14, 1);
    lcd.print("/5");
  } else if (currentSettingsItem == 2) {
    lcd.setCursor(12, 1);
    if (EEPROM.read(eepromSound)) {
      lcd.print("on");
    } else {
      lcd.print("off");
    }
  }

  lcd.setCursor(4, 0);
  lcd.print("Settings");
  lcd.setCursor(0, 1);
  lcd.print(settingsElements[currentSettingsItem]);

  // right
  if (xValue < minThreshold && joyMoved == false) {
    if (currentSettingsItem > 0) {
      currentSettingsItem--;
    }
    if (sound) {
      tone(buzzerPin, 3500, 5);
    }
    lcd.clear();
    joyMoved = true;
  }

  // left
  if (xValue > maxThreshold && joyMoved == false) {
    if (currentSettingsItem < settingsItems - 1) {
      currentSettingsItem++;
    }
    if (sound) {
      tone(buzzerPin, 3500, 5);
    }
    lcd.clear();
    joyMoved = true;
  }

  // neutral joystick
  if (xValue >= minThreshold && xValue <= maxThreshold) {
    joyMoved = false;
  }

  modifyBrightnessAndSound();
  readControlMenu(state);
}

void modifyBrightnessAndSound() {
  yValue = analogRead(pinY);

  // up
  if (yValue < minThreshold && brightnessJoyMoved == false) {
    if (currentSettingsItem == 0) {
      if (lcdBrightness < 5) {
        lcdBrightness++;
        EEPROM.write(eepromLcdBrightness, lcdBrightness);
      }
    } else if (currentSettingsItem == 1) {
      if (matrixBrightness < 5) {
        matrixBrightness++;
        EEPROM.write(eepromMatrixBrightness, matrixBrightness);
      }
    } else if (currentSettingsItem == 2) {
      EEPROM.write(eepromSound, 1);
    }
    if (sound) {
      tone(buzzerPin, 3500, 5);
    }
    lcd.clear();
    brightnessJoyMoved = true;
  }

  // down
  if (yValue > maxThreshold && brightnessJoyMoved == false) {
    if (currentSettingsItem == 0) {
      if (lcdBrightness > 0) {
        lcdBrightness--;
        EEPROM.write(eepromLcdBrightness, lcdBrightness);
      }
    } else if (currentSettingsItem == 1) {
      if (matrixBrightness > 0) {
        matrixBrightness--;
        EEPROM.write(eepromMatrixBrightness, matrixBrightness);
      }
    } else if (currentSettingsItem == 2) {
      EEPROM.write(eepromSound, 0);
    }

    if (sound) {
      tone(buzzerPin, 3500, 5);
    }
    lcd.clear();
    brightnessJoyMoved = true;
  }

  if (yValue >= minThreshold && yValue <= maxThreshold) {
    brightnessJoyMoved = false;
  }
}


// code - reset score
void resetScoreMenu(byte& state) {
  yValue = analogRead(pinY);

  lcd.setCursor(5, 0);
  lcd.print("Reset");
  lcd.setCursor(5, 1);
  lcd.print("Cancel");

  if (resetScoreFlag) {
    lcd.setCursor(4, 0);
    lcd.print(">");
  } else {
    lcd.setCursor(4, 1);
    lcd.print(">");
  }

  // up
  if (yValue < minThreshold && joyMoved == false) {
    if (sound) {
      tone(buzzerPin, 3500, 5);
    }
    lcd.clear();
    resetScoreFlag = true;
    joyMoved = true;
  }

  // down
  if (yValue > maxThreshold && joyMoved == false) {
    if (sound) {
      tone(buzzerPin, 3500, 5);
    }
    lcd.clear();
    resetScoreFlag = false;
    joyMoved = true;
  }

  if (yValue >= minThreshold && yValue <= maxThreshold) {
    joyMoved = false;
  }

  readControlMenu(state);
}


// code - leaderboard 
void leaderboardMenu(byte& state) {
  xValue = analogRead(pinX);

  if (currentRankItem == 0) {
    lcd.setCursor(15, 0);
    lcd.print(">");
  } else if (currentRankItem == leaderboardSize - 1) {
    lcd.setCursor(0, 0);
    lcd.print("<");
  } else {
    lcd.setCursor(0, 0);
    lcd.print("<");
    lcd.setCursor(15, 0);
    lcd.print(">");
  }

  lcd.setCursor(3, 0);
  lcd.print("Highscore");
  lcd.setCursor(0, 1);
  lcd.print(leaderboardPlaces[currentRankItem]);
  lcd.setCursor(5, 1);
  lcd.print(playerName);
  lcd.setCursor(12, 1);
  lcd.print(":");
  lcd.setCursor(14, 1);
  lcd.print(playerScore);

  // left
  if (xValue < minThreshold && joyMoved == false) {
    if (currentRankItem > 0) {
      currentRankItem--;
    }
    if (sound) {
      tone(buzzerPin, 3500, 5);
    }
    lcd.clear();
    joyMoved = true;
  }

  // right
  if (xValue > maxThreshold && joyMoved == false) {
    if (currentRankItem < leaderboardSize - 1) {
      currentRankItem++;
    }
    if (sound) {
      tone(buzzerPin, 3500, 5);
    }
    lcd.clear();
    joyMoved = true;
  }

  // neutral joystick
  if (xValue >= minThreshold && xValue <= maxThreshold) {
    joyMoved = false;
  }
  readControlMenu(state);
}


void scrollingMessage(char* Text, int TextLineNumber) {
  if (millis() - previousTime >= scrollTextTime) {
    int lengthOfText = strlen(Text);

    if (scrollingTextCursor == lengthOfText - 1) {
      scrollingTextCursor = 0;
    }
    lcd.setCursor(0, TextLineNumber);
    if (scrollingTextCursor < lengthOfText - 16) {
      for (int i = scrollingTextCursor; i < scrollingTextCursor + 16; i++) {
        lcd.print(Text[i]);
      }
    } else {
      for (int i = scrollingTextCursor; i < lengthOfText; i++) {
        lcd.print(Text[i]);
      }
      for (int i = 0; i <= 16 - (lengthOfText - scrollingTextCursor); i++) {
        lcd.print(Text[i]);
      }
    }
    scrollingTextCursor++;
    previousTime = millis();
  }
}


// code - menu controls
void readControlMenu(byte& state) {
  currentMillis = millis();

  if (currentMillis - previousButtonMillis > intervalButton) {
    buttonValue = digitalRead(pinSW);
    buttonValue = !buttonValue;

    if (buttonValue == HIGH && buttonStatePrevious == LOW && !buttonLongPressed) {
      buttonLongPressMillis = currentMillis;
      buttonStatePrevious = HIGH;
    }
    buttonPressDuration = currentMillis - buttonLongPressMillis;
    // long press
    if (buttonValue == HIGH && !buttonLongPressed && buttonPressDuration >= minButtonLongPressDuration) {
      if (sound) {
        tone(buzzerPin, 3500, 20);
      }
      if (menuPosition == 1) {
        if (enterUserNameDisplay) {
          enterUserNameDisplay = false;
          enterLevelDisplay = true;
          lcd.clear();
        }
      }
      buttonLongPressed = true;
    }
    if (buttonValue == LOW && buttonStatePrevious == HIGH) {
      buttonStatePrevious = LOW;
      buttonLongPressed = false;
      // short press
      if (!buttonLongPressed && buttonPressDuration < minButtonLongPressDuration) {
        if (sound) {
          tone(buzzerPin, 1500, 5);
        }
        Serial.println(state);
        if (menuPosition == 1) {
          if (state != START_GAME) {
            state = START_GAME;
            lcd.clear();
          } else if (enterLevelDisplay) {
            if (easyLevel) {
              dotRowPosition = 0;
              dotColPosition = 0;
            } else if (mediumLevel || hardLevel) {
              dotRowPosition = 1;
              dotColPosition = 1;
              rowSnakePosition[0] = 1;
              colSnakePosition[0] = 1;
            }

            up = false;
            down = false;
            right = false;
            left = false;
            endGame = false;

            enterLevelDisplay = false;
            gameDisplay = true;
            prevTimeGame = millis();
            lc.clearDisplay(0);
            lcd.clear();
          }
        }
        if (menuPosition == 2) {
          if (state != LEADERBOARD) {
            state = LEADERBOARD;
            lcd.clear();
          } else {
            state = MAIN_MENU;
            lcd.clear();
          }
        }
        if (menuPosition == 3) {
          if (state != SETTINGS) {
            state = SETTINGS;
            lcd.clear();
          } else {
            state = MAIN_MENU;
            lcd.clear();
          }
        }
        if (menuPosition == 4) {
          if (state != ABOUT) {
            state = ABOUT;
            scrollingTextCursor = defaultTextCursor;
            lcd.clear();
          } else {
            state = MAIN_MENU;
            lcd.clear();
          }
        }
        if (menuPosition == 5) {
          if (state != HOW_TO_PLAY) {
            state = HOW_TO_PLAY;
            scrollingTextCursor = defaultTextCursor;
            lcd.clear();
          } else {
            state = MAIN_MENU;
            lcd.clear();
          }
        }
        if (menuPosition == 6) {
          if (state != RESET_SCORE) {
            state = RESET_SCORE;
            lcd.clear();
          } else {
            if (resetScoreFlag) {
              for (int i = eepromScoreStart; i <= eepromScoreEnd; i++) {
                EEPROM.write(i, 0);
              }
            }
            state = MAIN_MENU;
            lcd.clear();
          }
        }
      }
    }
  }
}


// code - after game
void returnToMenu() {

  for (int i = 1; i <= deleteDotPosition; i++) {
    if (rowSnakePosition[0] == rowSnakePosition[i] && colSnakePosition[0] == colSnakePosition[i]) {
      endGame = true;
    }
  }

  if (mediumLevel || hardLevel) {
    if (rowSnakePosition[0] == 0 || rowSnakePosition[0] == (matrixSize - 1) || colSnakePosition[0] == 0 || colSnakePosition[0] == (matrixSize - 1)) {
      endGame = true;
    }
  }

  if (endGame) {
    if (newScore > EEPROM.read(eepromScoreStart)) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Congratulations!");
      lcd.setCursor(0, 1);
      lcd.print("New Highscore!!!");
      for (int i = 0; i < matrixSize; i++) {
        lc.setRow(0, i, matrixShapes[6][i]);
      }
      delay(2500);
      if(sound)
        song();
    } else {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Better luck");
      lcd.setCursor(0, 1);
      lcd.print("Next time :(");
      for (int i = 0; i < matrixSize; i++) {
        lc.setRow(0, i, matrixShapes[7][i]);
      }
      if(sound)
        song();
    }

    // scrolling matrix
    for (int i = 0; i < sizeof(gameOverMessage[0]) - (matrixSize - 1); i++) {
    for (int col = 0; col < matrixSize; col++) {
      delay(matrixMessageSpeed);
      for (int row = 0; row < matrixSize; row++) {
        lc.setLed(0, row, col, pgm_read_byte(&(gameOverMessage[row][col +i])));
      }
    }
  }

    // leaderboard update
    if (newScore > EEPROM.read(eepromScoreStart)) { //if first
      currentRankItem = 0;
      letterCounter = 0;

      // move the score down
      scoreToMap = EEPROM.read(eepromScoreStart);
      for (int i = eepromScoreStart + 1; i <= eepromScoreEnd; i++) {
        previousScore = EEPROM.read(i);
        EEPROM.write(i, scoreToMap);
        scoreToMap = previousScore;
      }
      EEPROM.write(eepromScoreStart, newScore);

      // move the rest down
      for (int i = eepromNameEnd; i >= eepromNameStep + eepromNameStart; i--) {
        letterToMap = EEPROM.read(i - eepromNameStep);
        EEPROM.write(i, letterToMap);
      }

      // new player to eeprom
      for (int i = eepromNameStart + (eepromNameStep * currentRankItem); i < eepromNameStep + (eepromNameStart + (eepromNameStep * currentRankItem)); i++) {
        EEPROM.write(i, alphabet[enterNameLetters[letterCounter]]);
        letterCounter++;
      }
    } else if (newScore > EEPROM.read(eepromScoreStart + 1)) { // if second
      currentRankItem = 1;
      letterCounter = 0;

      // move the score down
      scoreToMap = EEPROM.read(eepromScoreStart + 1);
      for (int i = eepromScoreStart + 2; i <= eepromScoreEnd; i++) {
        previousScore = EEPROM.read(i);
        EEPROM.write(i, scoreToMap);
        scoreToMap = previousScore;
      }
      EEPROM.write((eepromScoreStart + 1), newScore);

      // move the rest down
      for (int i = eepromNameEnd; i >= (eepromNameStep * (currentRankItem + 1)) + eepromNameStart; i--) {
        letterToMap = EEPROM.read(i - eepromNameStep);
        EEPROM.write(i, letterToMap);
      }

      // new player to eeprom
      for (int i = eepromNameStart + (eepromNameStep * currentRankItem); i < eepromNameStep + (eepromNameStart + (eepromNameStep * currentRankItem)); i++) {
        EEPROM.write(i, alphabet[enterNameLetters[letterCounter]]);
        letterCounter++;
      }
    } else if (newScore > EEPROM.read(eepromScoreStart + 2)) { // if third
      currentRankItem = 2;
      letterCounter = 0;

      // move the score down
      scoreToMap = EEPROM.read(eepromScoreStart + 2);
      for (int i = eepromScoreStart + 3; i <= eepromScoreEnd; i++) {
        previousScore = EEPROM.read(i);
        EEPROM.write(i, scoreToMap);
        scoreToMap = previousScore;
      }
      EEPROM.write((eepromScoreStart + 2), newScore);

      // move the rest down
      for (int i = eepromNameEnd; i >= (eepromNameStep * (currentRankItem + 1)) + eepromNameStart; i--) {
        letterToMap = EEPROM.read(i - eepromNameStep);
        EEPROM.write(i, letterToMap);
      }

      // new player to eeprom
      for (int i = eepromNameStart + (eepromNameStep * currentRankItem); i < eepromNameStep + (eepromNameStart + (eepromNameStep * currentRankItem)); i++) {
        EEPROM.write(i, alphabet[enterNameLetters[letterCounter]]);
        letterCounter++;
      }
    } else if (newScore > EEPROM.read(eepromScoreStart + 3)) { // if fourth
      currentRankItem = 3;
      letterCounter = 0;

      // move the score down
      scoreToMap = EEPROM.read(eepromScoreStart + 3);
      for (int i = eepromScoreStart + 4; i <= eepromScoreEnd; i++) {
        previousScore = EEPROM.read(i);
        EEPROM.write(i, scoreToMap);
        scoreToMap = previousScore;
      }
      EEPROM.write((eepromScoreStart + 3), newScore);

      // move the rest down
      for (int i = eepromNameEnd; i >= (eepromNameStep * (currentRankItem + 1)) + eepromNameStart; i--) {
        letterToMap = EEPROM.read(i - eepromNameStep);
        EEPROM.write(i, letterToMap);
      }

      // new player to eeprom
      for (int i = eepromNameStart + (eepromNameStep * currentRankItem); i < eepromNameStep + (eepromNameStart + (eepromNameStep * currentRankItem)); i++) {
        EEPROM.write(i, alphabet[enterNameLetters[letterCounter]]);
        letterCounter++;
      }
    } else if (newScore > EEPROM.read(eepromScoreStart + 4)) { // if fifth
      currentRankItem = 4;
      letterCounter = 0;

      // move the score down
      scoreToMap = EEPROM.read(eepromScoreStart + 2);
      for (int i = eepromScoreStart + 3; i <= eepromScoreEnd; i++) {
        previousScore = EEPROM.read(i);
        EEPROM.write(i, scoreToMap);
        scoreToMap = previousScore;
      }
      EEPROM.write((eepromScoreStart + 4), newScore);

      // move the rest down
      for (int i = eepromNameEnd; i >= (eepromNameStep * (currentRankItem + 1)) + eepromNameStart; i--) {
        letterToMap = EEPROM.read(i - eepromNameStep);
        EEPROM.write(i, letterToMap);
      }

      // new player to eeprom
      for (int i = eepromNameStart + (eepromNameStep * currentRankItem); i < eepromNameStep + (eepromNameStart + (eepromNameStep * currentRankItem)); i++) {
        EEPROM.write(i, alphabet[enterNameLetters[letterCounter]]);
        letterCounter++;
      }
    } else if (newScore > EEPROM.read(eepromScoreStart + 5)) { //if sixth
      currentRankItem = 5;
      letterCounter = 0;

      EEPROM.write((eepromScoreStart + 5), newScore);

      // new player to eeprom
      for (int i = eepromNameStart + (eepromNameStep * currentRankItem); i < eepromNameStep + (eepromNameStart + (eepromNameStep * currentRankItem)); i++) {
        EEPROM.write(i, alphabet[enterNameLetters[letterCounter]]);
        letterCounter++;
      }
    }

    // reset snake
    for (int i = 0; i <= deleteDotPosition; i++) {
      rowSnakePosition[i] = 0;
      colSnakePosition[i] = 0;
    }

    // reset name
    for (int i = 0; i < playerNameLength; i++) {
      enterNameLetters[i] = 0;
    }
    newPlayer = "";
    newScore = 0;
    deleteDotPosition = 0;

    state = MAIN_MENU;
    enterUserNameDisplay = true;
    gameDisplay = false;
    shouldGenerateFood = true;
    lcd.clear();
  }
}