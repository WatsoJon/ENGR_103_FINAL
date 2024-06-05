#include <Adafruit_CircuitPlayground.h>

int count = 0;

int pickCount = 0;
int betSelect = 0;
int switchState = 0;
int wins = 0;
int level = wins + 1;
int timing = 0;
int userBet = 0;
float X, Y, Z, totalAccel;
int houseBet = 0;
int levelOne = 3;
int levelTwo = 2;
int levelThree = 1;
int levelFour = 0;
int levelLights = 0;

volatile bool rightButtonFlag, leftButtonFlag = 0;
volatile bool switchFlag = 0;


void setup() {
  Serial.begin(9600);

  CircuitPlayground.begin();

  CircuitPlayground.setAccelRange(LIS3DH_RANGE_8_G);

  attachInterrupt(digitalPinToInterrupt(5), rightButtonISR, RISING);
  attachInterrupt(digitalPinToInterrupt(4), leftButtonISR, RISING);
  attachInterrupt(digitalPinToInterrupt(7), switchISR, CHANGE);

  randomSeed(analogRead(0));

}

void rightButtonISR () {
  rightButtonFlag = 1;
}

void leftButtonISR () {
  leftButtonFlag = 1;
}

void switchISR () {
  switchFlag = 1;
  switchState = !switchState;
}

void loop() {
  switchState = CircuitPlayground.slideSwitch();

  if(timing == 0) {
  Serial.print("You are on level, ");
  Serial.print(level);
  Serial.println(", make your bet selection using the right and left buttons!");
  }

  betPlace();

  if(switchState == 0) {
  Serial.print("You have chosen the # ");
  Serial.print(betSelect + 1);
  Serial.println(" LED, enable the switch, and shake the arduino when happy with your choice!");
  delay(500);
  timing += 1;
  }

  if(switchState == 1) {
    betSelect = 0;
    killLights();
    
    shakeSensor();
    if(totalAccel >= 50) {
      Serial.println("code here");
      rouletteSpin();
    }
  }



}


void betPlace() {
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

void betLights() {
  for(int i = -1; i < betSelect; ++i) {
    CircuitPlayground.setPixelColor(betSelect, 255, 255, 255);
    delay(50); 
    CircuitPlayground.setPixelColor(i, CircuitPlayground.colorWheel(0));
  }
}

void killLights() {
  for(int i = 0; i < 10; ++i) {
    CircuitPlayground.setPixelColor(i, CircuitPlayground.colorWheel(0)); 
  }
}

void killSomeLights() {
  for(int i = 10; i > betSelect; --i) {
    CircuitPlayground.setPixelColor(i, CircuitPlayground.colorWheel(0));
  }
}

void shakeSensor() {
  X = 0;
  Y = 0;
  Z = 0;
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
  Serial.println(totalAccel);
  delay(500);
}

void rouletteSpin() {
  houseBet = random(10);
  //houseBet = 3; 
  Serial.println(houseBet);


  for(int i = 0; i < 10; ++i) {
    CircuitPlayground.setPixelColor(i, 255, 255, 0);
    delay(50);
    CircuitPlayground.setPixelColor(i, CircuitPlayground.colorWheel(0));
    delay(10);
  }
  for(int i = 0; i < 10; ++i) {
    CircuitPlayground.setPixelColor(i, 255, 255, 0);
    delay(50);
    CircuitPlayground.setPixelColor(i, CircuitPlayground.colorWheel(0));
    delay(10);
  }
  for(int i = 0; i < houseBet; ++i) {
    CircuitPlayground.setPixelColor(i, 255, 255, 0);
    delay(200);
    CircuitPlayground.setPixelColor(i, CircuitPlayground.colorWheel(0));
  }


  switch(level){
  case 1:
  if(houseBet == 1) {
    for(int j = 7; j <= 10; ++j) {
    CircuitPlayground.setPixelColor(j, 255, 255, 0);
    delay(200);
    }
  }
  else if(houseBet == 2) {
    for(int h = 9; h <= 10; ++h) {
    CircuitPlayground.setPixelColor(h, 255, 255, 0);
    delay(200);
    }
  }
  else if(houseBet == 3) {
    for(int g = 9; g <= 10; ++g) {
      CircuitPlayground.setPixelColor(g, 255, 255, 0);
      delay(200);
    }
  }
  for(int a = houseBet - levelOne - 1; a < houseBet + levelOne; ++a) {
    CircuitPlayground.setPixelColor(a, 255, 255, 0);
    delay(200);
  }
  if(houseBet == 8) {
    for(int f = 0; f < 1; ++f) {
      CircuitPlayground.setPixelColor(f, 255, 255, 0);
      delay(200);
    }
  }
  else if(houseBet == 9) {
    for(int d = 0; d < 2; ++d) {
      CircuitPlayground.setPixelColor(d, 255, 255, 0);
      delay(200);
    }
  }
  else if(houseBet == 10) {
    for(int s = 0; s < 3; ++s) {
      CircuitPlayground.setPixelColor(s, 255, 255, 0); 
    }
  }


  if(houseBet == userBet || houseBet == userBet - 3 || houseBet == userBet - 2 || houseBet == userBet - 1 || houseBet == userBet + 1 || houseBet == userBet + 2 || houseBet == userBet + 3) {
    wins = wins + 1;
    for(int j = 0; j <= 3; ++j) {
    for(int i = 0; i <= 10; ++i) {
      CircuitPlayground.setPixelColor(i, 255, 255, 0);
      delay(50); 
      CircuitPlayground.setPixelColor(i, 0);
      delay(50);
    }
    } 
  }

  }


}

