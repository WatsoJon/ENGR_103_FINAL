// This is my final iteration of my hopefully working roulette code
// the goal of this second version is to organize my thoughts and 
// come accross cleaner through my logic flow

#include <Adafruit_CircuitPlayground.h>

// variables for main loop
int win = 0;
int timing = 0;
int level = 0;
int betSave = 0;
bool winCondition = false; 

// variables for betPlace
int pickCount, betSelect = 0;
int userBet, switchState = 0;

// variables for levelLoop
int levelOne = 3;
int levelTwo = 2;
int levelThree = 1;
int levelFour = 0;
int levelLights = 0;
int numLED = 10;

// variables for shakeSensor
float X, Y, Z, totalAccel = 0.0;

// variables for rouletteSpin
int houseBet = 0;

//ISR variables
volatile bool rightButtonFlag, leftButtonFlag = 0;
volatile bool switchFlag = 0;

// variables for music
float midi[127];
int winSong[6][2] = 
{
  {76, 100},
  {79, 100},
  {83, 100},
  {88, 100},
  {84, 100},
  {91, 100},
};

int loseSong[8][2] =
{
  {62, 100},
  {60, 100},
  {58, 100},
  {57, 100},
  {55, 100},
  {55, 100},
  {53, 100},
  {51, 100},
};
int A_four = 440;



void setup() {

  Serial.begin(9600);  //BAUD number

  CircuitPlayground.begin();    //initialize the circuit board

  //Attatch Interrupt ISRs appropriately
  attachInterrupt(digitalPinToInterrupt(5), rightButtonISR, RISING);
  attachInterrupt(digitalPinToInterrupt(4), leftButtonISR, RISING);
  attachInterrupt(digitalPinToInterrupt(7), switchISR, CHANGE);

  // Random Function to use for RouletteSpin
  randomSeed(analogRead(0));

  // Run the function for MIDI notes
  generateMIDI();

}

void rightButtonISR () {
  rightButtonFlag = 1;
}

void leftButtonISR () {
  leftButtonFlag = 1;
}

void switchISR() {
  switchFlag = 1;
  switchState = !switchState;
}

void loop() {
  switchState = CircuitPlayground.slideSwitch();
  
  level = win + 1; 

  betPlace();

  if(switchState == 0) {
  if(betSave != betSelect) {
    Serial.print("You have chosen the # ");
    Serial.print(betSelect + 1);
    Serial.println(" LED, enable the switch and shake the arduino when hapy with your choice!");
  }
  }

  if(switchState == 1) {
    killLights();

    shakeSensor();
    if(totalAccel >= 20) {
      rouletteSpin();
      Serial.print("How exciting!");
      Serial.print(" The house chooses LED # ");
      Serial.print(houseBet + 1);
      Serial.println("!");

      levelLoop(); 
      
    }
    switchFlag = 0;
  }

  delay(500);
}



void betPlace() {
  betSave = betSelect;
  if(rightButtonFlag == 1) {
    delay(20);
    if(betSelect + 1 != 10) {
      betSelect = betSelect + 1;
    }
    killSomeLights();
    betLights();
    rightButtonFlag = 0;
  }

  if(leftButtonFlag == 1) {
    delay(20);
    if(betSelect - 1 != -1) {
      betSelect = betSelect - 1;
    }
    killSomeLights();
    betLights(); 
    leftButtonFlag = 0;

  }

  userBet = betSelect + 1;
}



void killSomeLights() {
  for(int i = 10; i > betSelect; --i) {
    CircuitPlayground.setPixelColor(i, 0, 0, 100);
  }
}

void killLights() {
  for(int i = 0; i < 10; ++i) {
    CircuitPlayground.setPixelColor(i, 0, 0, 100);
    delay(5);
  }
}

void betLights() {
  for(int i = -1; i < betSelect; ++i) {
    CircuitPlayground.setPixelColor(betSelect, 255, 255, 255);
    CircuitPlayground.setPixelColor(i, 0, 0, 100);
  }
}



void shakeSensor() {

  for (int i=0; i<10; i++) {
    X += CircuitPlayground.motionX();
    Y += CircuitPlayground.motionY();
    Z += CircuitPlayground.motionZ();
    delay(1);
  }
  X /= 10;
  Y /= 10;
  Z /= 10;


  totalAccel = sqrt(X*X + Y*Y + Z*Z);
  delay(500);
}



void rouletteSpin() {
  Serial.println("spinning....");
  houseBet = random(9);

  for(int i = 0; i < 5; ++i) {
  for(int j = 0; j < 10; ++j) {
    CircuitPlayground.setPixelColor(j, 255, 255, 255);
    delay(50);
    CircuitPlayground.setPixelColor(j, 0, 0, 100);
  }
  }
  for(int i = 0; i < houseBet; ++i) {
    CircuitPlayground.setPixelColor(i, 255, 255, 255);
    delay(50);
    CircuitPlayground.setPixelColor(i, 0, 0, 100);
  }
  CircuitPlayground.setPixelColor(houseBet, 0, 200, 200);
  delay(1000);
  killLights();
}

void levelLoop() {
  switch (level) {
  case 1:
    winCondition = isGuessWithinRange(betSelect, houseBet, 3);
    break;
  case 2:
    winCondition = isGuessWithinRange(betSelect, houseBet, 2);
    break;
  case 3:
    winCondition = isGuessWithinRange(betSelect, houseBet, 1);
    break;
  case 4:
    winCondition = isGuessWithinRange(betSelect, houseBet, 0);
    break;
  }

  if(winCondition) {
    win++;
    Serial.println("You win!");
  } else {
    Serial.println("You lose!");
    level = 1;
    win = 0;
  }

  illuminateCorrectLEDs(winCondition);
}

bool isGuessWithinRange(int guess, int correct, int range) {
  int min = (correct - range + numLED) % numLED;
  int max = (correct + range) % numLED;

  if(range == 0) {
    return guess == correct;
  }

  if (min < max) {
    return guess >= min && guess <= max;
  } else {
    return guess >= min || guess <= max;
  }
}

void illuminateCorrectLEDs(bool winCondition) {
  int range = 0;
  switch (level) {
    case 1:
      range = 3;
      break;
    case 2:
      range = 2;
      break;
    case 3:
      range = 1;
      break;
    case 4:
      range = 0;
      break;
    case 5:
      range = 0;
      break;
    case 6:
      finishLoop();
  }

  int min = (houseBet - range + numLED) % numLED;
  int max = (houseBet + range) % numLED;

  if (winCondition) {
    if (range == 0) {
      CircuitPlayground.setPixelColor(houseBet, 0, 255, 0); // Green for correct LED
    } else {
      if (min < max) {
        for (int i = min; i <= max; i++) {
          CircuitPlayground.setPixelColor(i, 0, 255, 0); // Green for correct LEDs
          delay(100);
        }
      } else {
        for (int i = min; i < numLED; i++) {
          CircuitPlayground.setPixelColor(i, 0, 255, 0); // Green for correct LEDs
          delay(100);
        }
        for (int i = 0; i <= max; i++) {
          CircuitPlayground.setPixelColor(i, 0, 255, 0); // Green for correct LEDs
          delay(100);
        }
      }
    }
    winSongLoop();
  } else {
    for (int i = 0; i < 10; i++) {
      CircuitPlayground.setPixelColor(i, 255, 0, 0); // Red for fail
      delay(50);
    }
    loseSongLoop();
  }

  if (winCondition) {
    Serial.print("You are on level ");
    Serial.print(level + 1);
    Serial.print(". You currently have ");
    Serial.print(win);
    Serial.println(" win(s), congratulations!");
    Serial.println("Flip the switch when you're ready to try again!");
  } else {
    Serial.println("Nice try, NOT! Flip the switch and try again!");
  }
}

void finishLoop() {
  win = 0;
  level = 0;
  Serial.println("Congratulations for completing the game!");
  Serial.println("You can flip the switch whenever and restart the game!");

  for(int j = 0; j < 10; ++j) {
    for(int i = 0; i < 10; ++i) {
      CircuitPlayground.setPixelColor(i, 0, 255, 0);
      delay(100);
      CircuitPlayground.setPixelColor(i, 0, 0, 0);
    }
  }
}


void generateMIDI()
{
  for (int x = 0; x < 127; ++x)
  {
    midi[x] = (A_four / 32.0) * pow(2.0, ((x - 9.0) / 12.0));
    Serial.println(midi[x]);
  }
}

void winSongLoop() {
  delay(100);
  for(int i = 0; i < sizeof(winSong) / sizeof(winSong[0]); i++) 
  {
    CircuitPlayground.playTone(midi[winSong[i][0]], winSong[i][1]);
    Serial.println(midi[winSong[i][0]]);
    delay(1);
  }
}

void loseSongLoop() {
  delay(100);
  for(int i = 0; i < sizeof(loseSong) / sizeof(loseSong[0]); i++) 
  {
    CircuitPlayground.playTone(midi[loseSong[i][0]], loseSong[i][1]);
    Serial.println(midi[loseSong[i][0]]);
    delay(1);
  }
}












