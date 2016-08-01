#include <phys253.h>
#include "pins.hpp"
#include "Arm_And_Stepper.hpp"
#include "TapeFollow.hpp"
#include "CollisionWatch.hpp"
#include "PassengerSeek.hpp"
#include "ToDestination.hpp"

const int FIND_PASSENGER = 0;
const int LOAD_PASSENGER_LEFT = 1;
const int LOAD_PASSENGER_RIGHT = 2;
const int FIND_BEACON = 3;
const int DROP_PASSENGER = 4;

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

  //Arm & Stepper Initialization code
  pinMode(DIR_PIN,OUTPUT);
  pinMode(PULSE_PIN,OUTPUT);
  baseTarget = BASE_REST_POSITION;
  midTarget = MID_REST_POSITION;
  lastPropErr = 0.;
}

void loop() {
  if(startbutton()){
    started = true;
    tapeFollowInit();
    PassengerSeek::init();
    tapeFollowStart();
    LCD.clear();
  }
  if(stopbutton()){
    tapeFollowTest();
    PassengerSeek::pause();
    LCD.clear();
    LCD.print("stopped!");
  }
  if(started){
    doControl(); //Can't not do this or the arm will FUCKING EXPLODE
    
    if(state == FIND_PASSENGER){
      findPassengerLoop();
    } else if(state == LOAD_PASSENGER_LEFT){
      turnAndReach(false, true);
    } else if(state == LOAD_PASSENGER_RIGHT){
      turnAndReach(true, true);
    }
  }
}

void findPassengerLoop(){
    tapeFollowLoop();
    collisionLoop();
    PassengerSeek::loop();
    if(PassengerSeek::isAtPassenger()){
      tapeFollowTest();
      /*int side = PassengerSeek::getPassengerSide();
      if(side == 1){
        state = LOAD_PASSENGER_RIGHT;
      }else if(side == -1){
        state = LOAD_PASSENGER_LEFT;
      }
      PassengerSeek::stop();*/
      //TODO: uncomment this once passenger seeking is working
    }
    if(hasDetectedCollision()){
      turnAround();
    }
}

