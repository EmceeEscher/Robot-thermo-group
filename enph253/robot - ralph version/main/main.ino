#include <phys253.h>
#include "pins.hpp"

const int FIND_PASSENGER = 0;
const int LOAD_PASSENGER = 1;
const int FIND_BEACON = 2;
const int DROP_PASSENGER = 3;

bool started = false;
int state = FIND_PASSENGER;


void setup() {
  // put your setup code here, to run once:
  #include <phys253setup.txt>
  RCServo1.detach();
  RCServo2.detach();
  Serial.begin(9600);
  randomSeed(analogRead(0));
  LCD.clear();
  LCD.print("Press START to");
  LCD.setCursor(0, 1);
  LCD.print("begin");
}

void loop() {
  if(startbutton()&&!started){
    started = true;
  }
  else if(stopbutton()&&started){
    started = false;
  }
  if(started){
    if(state == FIND_PASSENGER){
      findPassengerLoop();
    }
  }
}

void findPassengerLoop(){
  
}

