#include <Adafruit_CircuitPlayground.h>

const int switchPin = 7;

const uint32_t redcolor = 0xFF0000; // Left button color (red)
const uint32_t bluecolor = 0x0000FF; // Right button color (blue)

volatile bool intFlag1 = 0; // For switch
volatile bool intFlag2 = 0; // For left Button
volatile bool intFlag3 = 0; // For right Button

int score = 0;//score max 10
int roundNum = 0;//Number of rounds up to 10

const int lightDuration = 800; // Length of time ligh appears each time (0.8 seconds)

int activeLED = -1;
int activeColor = 0;

bool gameActive = false;
unsigned long lightStartTime = 0;

// Sound setup
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

//setup
void setup() {
  Serial.begin(9600);
  CircuitPlayground.begin();
  generateMIDI();
  pinMode(switchPin, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(switchPin), actual_switch, CHANGE);

  CircuitPlayground.clearPixels();
  nextRound();
}

void loop() {
  score = constrain(score, 0, 10);

  static bool prevLeft = false;
  static bool prevRight = false;

  bool currLeft = CircuitPlayground.leftButton();
  bool currRight = CircuitPlayground.rightButton();

  if (currLeft && !prevLeft) {
    intFlag2 = 1;
  }
  if (currRight && !prevRight) {
    intFlag3 = 1;
  }

  prevLeft = currLeft;
  prevRight = currRight;

  //Serial.print("isAnyLightOn: ");
  //Serial.println(isAnyLightOn());
  //Serial.print("intFlag2: ");
  //Serial.println(intFlag2);
  //Serial.print("intFlag3: ");
  //Serial.println(intFlag3);
  Serial.print("activeColor: ");
  Serial.println(activeColor);
  if (intFlag2) Serial.println("Left button detected");
  if (intFlag3) Serial.println("Right button detected");

  //if (isAnyLightOn() == false && roundNum < 10) {// button clicked when light is not on
    //if (intFlag2 || intFlag3) {
      //score--;
      //for(int i = 0; i < sizeof(lowtone) / sizeof(lowtone[1]); i++) // Calculate how many rows are in the array using: sizeof(song) / sizeof(song[0])
      //{
      //CircuitPlayground.playTone(midi[lowtone[i][0]], lowtone[i][1]);
      //}
      //Serial.print("Wrong Timing Score: ");
      //Serial.println(score);
      //intFlag2 = 0;
      //intFlag3 = 0;
    //}
  //}

  if (roundNum < 10 && isAnyLightOn()) {
    // Correct Presses
    if (intFlag3 && activeColor != 0) { // Right button, blue light
      score++;
      for(int i = 0; i < sizeof(hightone) / sizeof(hightone[1]); i++) // Calculate how many rows are in the array using: sizeof(song) / sizeof(song[0])
      {
      Serial.print("Playing tone: ");
      CircuitPlayground.playTone(midi[hightone[i][0]], hightone[i][1]);
      }
      Serial.print("Correct! Score: ");
      Serial.println(score);
      intFlag2 = 0;
      intFlag3 = 0;
      roundNum++;
      nextRound();
    }
    if (intFlag2 && activeColor == 0) { // Left button, red light
      score++;
      for(int i = 0; i < sizeof(hightone) / sizeof(hightone[1]); i++) // Calculate how many rows are in the array using: sizeof(song) / sizeof(song[0])
      {
      Serial.print("Playing tone: ");
      CircuitPlayground.playTone(midi[hightone[i][0]], hightone[i][1]);
      }
      Serial.print("Correct! Score: ");
      Serial.println(score);
      intFlag2 = 0;
      intFlag3 = 0;
      roundNum++;
      nextRound();
    }

    // Incorrect Presses
    if (intFlag3 && activeColor == 0) { // Right button, red light
      score--;
      for(int i = 0; i < sizeof(lowtone) / sizeof(lowtone[1]); i++) // Calculate how many rows are in the array using: sizeof(song) / sizeof(song[0])
      {
      Serial.print("Playing tone: ");
      CircuitPlayground.playTone(midi[lowtone[i][0]], lowtone[i][1]);
      }
      Serial.print("Incorrect! Score: ");
      Serial.println(score);
      intFlag2 = 0;
      intFlag3 = 0;
      roundNum++;
      nextRound();
    }
    if (intFlag2 && activeColor != 0) { // Left button, blue light
      score--;
      for(int i = 0; i < sizeof(lowtone) / sizeof(lowtone[1]); i++) // Calculate how many rows are in the array using: sizeof(song) / sizeof(song[0])
      {
      Serial.print("Playing tone: ");
      CircuitPlayground.playTone(midi[lowtone[i][0]], lowtone[i][1]);
      }
      Serial.print("Incorrect! Score: ");
      Serial.println(score);
      intFlag2 = 0;
      intFlag3 = 0;
      roundNum++;
      nextRound();
    }
  }

  // Time's up without press
  if (millis() - lightStartTime > lightDuration && isAnyLightOn()) {
    CircuitPlayground.clearPixels();
    activeLED = -1;
    roundNum++;

    if (roundNum < 10) {
      nextRound();
    }
  }

  // End game condition
  if (roundNum >= 10) {
    score = constrain(score, 0, 10);
    CircuitPlayground.clearPixels();

    for (int i = 0; i < score; i++) {
      CircuitPlayground.setPixelColor(i, 0x00FF00); // Show score in green
    }
    CircuitPlayground.strip.show();

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
  CircuitPlayground.strip.show();
  lightStartTime = millis(); //Light begins after this milliseconds after board started
}

// Check if any LED is on
bool isAnyLightOn() {
  for (int i = 0; i < 10; i++) {
    if (CircuitPlayground.strip.getPixelColor(i) != 0x000000) { //When any light is on
      return true;
    }
  }
  return false;
}

// Interrupt for switch
void actual_switch() {
  intFlag1 = 1;
}
