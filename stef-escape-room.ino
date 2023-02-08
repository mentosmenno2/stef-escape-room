// Include libraries
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <math.h>

// Include custom libraries
#include "pitches.h"

// Define pins
#define PIN_PIEZO 11
#define PIN_BUTTON_RESET 8
#define PIN_BUTTON_ADD 7
#define PIN_BUTTON_NEXT 6

// Define display constants
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Define global game variables
int levelNumber = 0;

void setup() {
  // Setup the serial monitor
  Serial.begin(9600);

  // Setup the display
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  display.setTextColor(SSD1306_WHITE);

  // Setup the buttons
  pinMode(PIN_BUTTON_RESET, INPUT);
  pinMode(PIN_BUTTON_ADD, INPUT);
  pinMode(PIN_BUTTON_NEXT, INPUT);

  // Setup the speaker
  pinMode(PIN_PIEZO, OUTPUT);
}

void loop() {
  playCurrentLevel();
  levelNumber++;
}

void playCurrentLevel() {
  switch (levelNumber) {
    case 0:
      playLevelZero();
      break;
    case 1:
      displayLevelMessage();
      playLevelOne();
      break;
    case 2:
      displayLevelMessage();
      playLevelTwo();
      break;
    case 3:
      displayLevelMessage();
      playLevelThree();
      break;
    case 4:
      displayLevelMessage();
      playLevelFour();
      break;
    case 5:
      displayLevelMessage();
      playLevelFive();
      break;
    default:
      playLevelFinished();
      break;
  }
}

// Levels

void playLevelFinished() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("Letter ontgrendeld!");
  display.setCursor(0, 16);
  display.setTextSize(2);
  display.println("Letter: E");
  display.display();

  playSoundFinish();

  int userInput = awaitUserInput();
  if(userInput == 0) {
    levelNumber = -1;
    return;
  }
  
  playLevelFinished();
}

void playLevelFive() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("Welke maand is WCEU?");
  display.display();

  awaitCorrectAnswer(6);
}

void playLevelFour() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("Hoe groot is onze\r\nstandaard grid gap?");
  display.display();

  awaitCorrectAnswer(32);
}

void playLevelThree() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("Hoeveel collega\r\ndesigners heb je?");
  display.display();

  awaitCorrectAnswer(4);
}

void playLevelTwo() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("Wat is de minimale\r\nlettergrootte die\r\naan a11y regels kan\r\nvoldoen?");
  display.display();

  awaitCorrectAnswer(14);
}

void playLevelOne() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("Hoeveel kernwaardes\r\nheeft Level level?");
  display.display();

  awaitCorrectAnswer(3);
}

void playLevelZero() {
  for (int percent = 0; percent < 100; percent++) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(100, 0);
    display.println(String(percent + 1) + "%");
    float lineX2 = round(((float)percent + (float)1) / (float)100 * ((float)display.width() - (float)1 ));
    Serial.println(lineX2);
    display.fillRect(0, display.height() - 6, lineX2, 5, SSD1306_WHITE);
    display.display();
  }

  playSoundLoaded();
  delay(1000);

  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(0, 0);
  display.println(F("Welkom\r\nStef"));
  display.display();
  delay(5000);

  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(1);
  display.println(F("Beantwoord al deze\r\nvragen om een letter\r\nte ontgrendelen."));
  display.display();
  delay(8000);
}

// Await input logic

void awaitCorrectAnswer(int correctAnswer) {
  int userInput = awaitUserInput();
  
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(1);
  display.println("Jouw antwoord");
  display.setCursor(0, 8);
  display.setTextSize(2);
  display.println("\"" + String(userInput) + "\"");
  display.setCursor(0, 24);
  display.setTextSize(1);
  display.println("is ...");
  display.display();
  delay(2000);
  if( userInput == correctAnswer ) {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.setTextSize(2);
    display.println(F("Goed! :)"));
    display.display();
    playSoundSuccess();
    delay(2000);
    return;
  }

  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(2);
  display.println(F("Fout :("));
  display.display();
  playSoundFail();
  delay(2000);
  levelNumber--; // Reset question to have completed previous. New loop will upp the level number.
}

int awaitUserInput() {
  int currentInput = 0;
  
  // Only enable after pressing one of the buttons
  while(digitalRead(PIN_BUTTON_RESET) == LOW && digitalRead(PIN_BUTTON_ADD) == LOW && digitalRead(PIN_BUTTON_NEXT) == LOW) {
    delay(1);
  }
  
  displayCurrentInputValue(currentInput);
  delay(200);
  
  while(digitalRead(PIN_BUTTON_NEXT) == LOW) {
      if (digitalRead(PIN_BUTTON_RESET) == HIGH) {
        currentInput = 0;
        displayCurrentInputValue(currentInput);
        delay(200);
      }
      if (digitalRead(PIN_BUTTON_ADD) == HIGH) {
        currentInput++;
        displayCurrentInputValue(currentInput);
        delay(200);
      }
  }

  return currentInput;
}

// Display messages

void displayLevelMessage() {
  display.clearDisplay();
  display.setCursor(30, 0);
  display.setTextSize(2);
  display.println("Vraag " + String(levelNumber));
  display.display();
  display.startscrollleft(0x00, 0x0F);
  delay(5000);
  display.stopscroll();
}

void displayCurrentInputValue(int input) {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(1);
  display.println("Jouw antwoord:");
  display.setCursor(0, 16);
  display.setTextSize(2);
  display.println(String(input));
  display.display();
}

// Play sounds

void playSoundSuccess() {
  tone(PIN_PIEZO, NOTE_E6, 125);
  delay(130);
  tone(PIN_PIEZO, NOTE_G6, 125);
  delay(130);
  tone(PIN_PIEZO, NOTE_E7, 125);
  delay(130);
  tone(PIN_PIEZO, NOTE_C7, 125);
  delay(130);
  tone(PIN_PIEZO, NOTE_D7, 125);
  delay(130);
  tone(PIN_PIEZO, NOTE_G7, 125);
  delay(125);
  noTone(PIN_PIEZO);
}

void playSoundFail() {
  tone(PIN_PIEZO, NOTE_B5, 200);
  delay(260);
  tone(PIN_PIEZO, NOTE_F6, 200);
  delay(300);
  tone(PIN_PIEZO, NOTE_F6, 150);
  delay(200);
  tone(PIN_PIEZO, NOTE_F6, 150);
  delay(200);
  tone(PIN_PIEZO, NOTE_E6, 200);
  delay(200);
  tone(PIN_PIEZO, NOTE_D6, 150);
  delay(200);
  tone(PIN_PIEZO, NOTE_C6, 150);
  delay(200);
  tone(PIN_PIEZO, NOTE_E5, 150);
  delay(200);
  tone(PIN_PIEZO, NOTE_E5, 150);
  delay(200);
  tone(PIN_PIEZO, NOTE_C5, 200);
  delay(200);
  
  noTone(PIN_PIEZO);
}

void playSoundLoaded() {
  tone(PIN_PIEZO, NOTE_B6, 100);
  delay(100);
  tone(PIN_PIEZO, NOTE_E7, 850);
  delay(800);
  noTone(PIN_PIEZO);
}

void playSoundFinish() {
  tone(PIN_PIEZO, NOTE_G3);
  delay(166);
  tone(PIN_PIEZO, NOTE_C4);
  delay(166);
  tone(PIN_PIEZO, NOTE_E4);
  delay(166);
  tone(PIN_PIEZO, NOTE_G4);
  delay(166);
  tone(PIN_PIEZO, NOTE_C5);
  delay(166);
  tone(PIN_PIEZO, NOTE_E5);
  delay(166);
  tone(PIN_PIEZO, NOTE_G5);
  delay(500);
  tone(PIN_PIEZO, NOTE_E5);
  delay(500);
 
  tone(PIN_PIEZO, NOTE_E3);
  delay(166);
  tone(PIN_PIEZO, NOTE_C4);
  delay(166);
  tone(PIN_PIEZO, NOTE_DS4);
  delay(166);
  tone(PIN_PIEZO, NOTE_GS4);
  delay(166);
  tone(PIN_PIEZO, NOTE_C5);
  delay(166);
  tone(PIN_PIEZO, NOTE_DS5);
  delay(166);
  tone(PIN_PIEZO, NOTE_GS5);
  delay(500);
  tone(PIN_PIEZO, NOTE_DS5);
  delay(500);
 
  tone(PIN_PIEZO, NOTE_AS3);
  delay(166);
  tone(PIN_PIEZO, NOTE_D4);
  delay(166);
  tone(PIN_PIEZO, NOTE_F4);
  delay(166);
  tone(PIN_PIEZO, NOTE_AS4);
  delay(166);
  tone(PIN_PIEZO, NOTE_D5);
  delay(166);
  tone(PIN_PIEZO, NOTE_F5);
  delay(166);
  tone(PIN_PIEZO, NOTE_AS5);
  delay(500);
  noTone(PIN_PIEZO);
  delay(50);
  tone(PIN_PIEZO, NOTE_AS5);
  delay(166);
  noTone(PIN_PIEZO);
  delay(50);
  tone(PIN_PIEZO, NOTE_AS5);
  delay(166);
  noTone(PIN_PIEZO);
  delay(50);
  tone(PIN_PIEZO, NOTE_AS5);
  delay(166);
  tone(PIN_PIEZO, NOTE_C6);
  delay(1000);
  noTone(PIN_PIEZO);
}
