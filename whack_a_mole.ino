#include <Adafruit_CircuitPlayground.h>

const int switchPin = 7;
const int LeftButton = 4;
const int RightButton = 5;

const uint32_t redcolor = 0xFF0000;
const uint32_t bluecolor = 0x0000FF;

volatile bool intFlag1 = 0; // Switch
volatile bool intFlag2 = 0; // Left Button
volatile bool intFlag3 = 0; // Right Button

int score = 0;
int roundNum = 0;

const int lightDuration = 800; 

int activeLED = -1;
int activeColor = 0;

bool gameActive = false;
unsigned long lightStartTime = 0;

float sound;
float midi[127];
int A_four = 440;

int c_major[8] = {60, 62, 64, 65, 67, 69, 71, 72};
int c_minor[8] = {60, 62, 63, 65, 67, 68, 70, 72};

int lowtone[1][2] = {
    {60, 150},
};

int hightone[1][2] = {
    {79, 150},
};

void setup() {
  Serial.begin(9600);
  CircuitPlayground.begin();
  generateMIDI();
  pinMode(LeftButton, INPUT_PULLUP);
  pinMode(RightButton, INPUT_PULLUP);
  pinMode(switchPin, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(switchPin), actual_switch, CHANGE);
  attachInterrupt(digitalPinToInterrupt(LeftButton), blink1, CHANGE);
  attachInterrupt(digitalPinToInterrupt(RightButton), blink2, CHANGE);

  CircuitPlayground.clearPixels();
  nextRound();
}

void loop() {
  score = constrain(score, 0, 10);
  if (isAnyLightOn()) {
    // Correct Presses
    if (intFlag3 && activeColor == 1) { // Right button, blue light
      score++;
      for(int i = 0; i < sizeof(hightone) / sizeof(hightone[1]); i++) // Calculate how many rows are in the array using: sizeof(song) / sizeof(song[0])
      {
      CircuitPlayground.playTone(midi[hightone[i][0]], hightone[i][1]);
      }
      Serial.print("Correct! Score: ");
      Serial.println(score);
      intFlag2 = 0;
      intFlag3 = 0;
      nextRound();
    }
    if (intFlag2 && activeColor == 0) { // Left button, red light
      score++;
      for(int i = 0; i < sizeof(hightone) / sizeof(hightone[1]); i++) // Calculate how many rows are in the array using: sizeof(song) / sizeof(song[0])
      {
      CircuitPlayground.playTone(midi[hightone[i][0]], hightone[i][1]);
      }
      Serial.print("Correct! Score: ");
      Serial.println(score);
      intFlag2 = 0;
      intFlag3 = 0;
      nextRound();
    }

    // Incorrect Presses
    if (intFlag3 && activeColor == 0) { // Right button, red light
      score--;
      for(int i = 0; i < sizeof(lowtone) / sizeof(lowtone[1]); i++) // Calculate how many rows are in the array using: sizeof(song) / sizeof(song[0])
      {
      CircuitPlayground.playTone(midi[lowtone[i][0]], lowtone[i][1]);
      }
      Serial.print("Incorrect! Score: ");
      Serial.println(score);
      intFlag2 = 0;
      intFlag3 = 0;
      nextRound();
    }
    if (intFlag2 && activeColor == 1) { // Left button, blue light
      score--;
      for(int i = 0; i < sizeof(lowtone) / sizeof(lowtone[1]); i++) // Calculate how many rows are in the array using: sizeof(song) / sizeof(song[0])
      {
      CircuitPlayground.playTone(midi[lowtone[i][0]], lowtone[i][1]);
      }
      Serial.print("Incorrect! Score: ");
      Serial.println(score);
      intFlag2 = 0;
      intFlag3 = 0;
      nextRound();
    }
  }

  // Late press after light disappears
  if (!isAnyLightOn()) {
    if (intFlag2 || intFlag3) {
      score--;
      for(int i = 0; i < sizeof(lowtone) / sizeof(lowtone[1]); i++) // Calculate how many rows are in the array using: sizeof(song) / sizeof(song[0])
      {
      CircuitPlayground.playTone(midi[lowtone[i][0]], lowtone[i][1]);
      }
      Serial.print("Too late! Score: ");
      Serial.println(score);
      intFlag2 = 0;
      intFlag3 = 0;
    }
  }

  // Time's up without press
  if (millis() - lightStartTime > lightDuration && isAnyLightOn()) {
    CircuitPlayground.clearPixels();
    roundNum++;

    if (roundNum < 10) {
      nextRound();
    }
  }

  // End game condition
  if (roundNum == 10) {
    score = constrain(score, 0, 10);
    CircuitPlayground.clearPixels();

    if (score == 1) {
      for (int i = 0; i < score; i++) {
        CircuitPlayground.setPixelColor(i, 0x00FF00); // Green LEDs for score
      }
    }

    CircuitPlayground.strip.show(); // REQUIRED to show pixels
    Serial.print("Final Score: ");
    Serial.println(score);
    Serial.println("Flip the switch to play again.");

    roundNum++;
  }

  // Handle reset switch
  if (intFlag1) {
    score = 0;
    roundNum = 0;
    CircuitPlayground.clearPixels();
    Serial.println("Game Reset!");
    delay(300);
    nextRound();
    intFlag1 = 0;
  }
}

// Generates MIDI note frequencies
void generateMIDI() {
  for (int x = 0; x < 127; ++x) {
    midi[x] = (A_four / 32.0) * pow(2.0, ((x - 9.0) / 12.0));
  }
}

// Show a new random light
void nextRound() {
  CircuitPlayground.clearPixels();
  delay(random(800, 1500));

  activeLED = random(0, 10);
  activeColor = random(0, 2); // 0 = red, 1 = blue

  CircuitPlayground.setPixelColor(
    activeLED,
    activeColor == 0 ? redcolor : bluecolor
  );

  lightStartTime = millis();
}

// Check if any LED is on
bool isAnyLightOn() {
  for (int i = 0; i < 10; i++) {
    if (CircuitPlayground.strip.getPixelColor(i) != 0x000000) {
      return true;
    }
  }
  return false;
}

// Interrupt handlers
void actual_switch() {
  intFlag1 = 1;
}

void blink1() {
  intFlag2 = 1;
}

void blink2() {
  intFlag3 = 1;
}
