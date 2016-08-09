//Something worked!!

#include <phys253.h>
#include "pins.hpp"
#include "ArmAndStepper.hpp"
#include "TapeFollow.hpp"
#include "CollisionWatch.hpp"
#include "PassengerSeek.hpp"
#include "FindBeacon.hpp"

const int FIND_PASSENGER = 0;
const int LOAD_PASSENGER_LEFT = 1;
const int LOAD_PASSENGER_RIGHT = 2;
const int FIND_BEACON = 3;
const int DROP_PASSENGER = 4;

const int PRINT_PERIOD {200};

bool started = false;
int state = FIND_PASSENGER;
int printCount = 0;

void setup() {
    // put your setup code here, to run once:
#include <phys253setup.txt>
    RCServo1.detach();
    RCServo2.detach();
    Serial.begin(9600);
    randomSeed(analogRead(0));
    LCD.clear();
    LCD.print("START: compete");
    LCD.setCursor(0, 1);
    LCD.print("STOP: debug");

    for(auto i(0); i < 16; i++){
      pinMode(i, INPUT_PULLUP);
    }

    ArmAndStepper::init();
}

void loop() {
  if(!started && startbutton()){
    //debugSequence();
    started = true;
    tapeFollowInit();
    PassengerSeek::init();
    tapeFollowStart();
    FindBeacon::init();
    LCD.clear();
  }
  if(!started && stopbutton()){
    debugSequence();
    tapeFollowInit();
    PassengerSeek::init();
    tapeFollowStart();
    FindBeacon::init();
    LCD.clear();
  }
  if(startbutton() && started){
      tapeFollowStart();
      PassengerSeek::init();
      FindBeacon::init();
      LCD.clear();
      
  }
  if(stopbutton() && started){
        tapeFollowTest();
        PassengerSeek::pause();
        LCD.clear();
        LCD.print("stopped!");
  }

  if (started && printCount % PRINT_PERIOD == 0) {
        if(state == FIND_PASSENGER){
          PassengerSeek::printLCD();
        }else if(state == FIND_BEACON)
          FindBeacon::printLCD();       
        printCount = 0;
    }
    ++printCount;
  if(started){
        ArmAndStepper::doControl(); //Can't not do this or the arm will EXPLODE
        FindBeacon::loop();
        if(state == FIND_PASSENGER){
            findPassengerLoop();
        } else if(state == LOAD_PASSENGER_LEFT){
            loadPassengerLoop();
        } else if(state == LOAD_PASSENGER_RIGHT){
            loadPassengerLoop();
        } else if(state == FIND_BEACON){
            findBeaconLoop();
        } else if(state == DROP_PASSENGER){
            dropPassengerLoop();
        }
    }
}

void findPassengerLoop(){
    tapeFollowLoop();
    CollisionWatch::loop();
    PassengerSeek::loop();
    if(PassengerSeek::isAtPassenger()){
        tapeFollowTest();
        PassengerSeek::pause();
        tapeFollowLoop();
        int side = PassengerSeek::getPassengerSide();
        if(side == 1){
          state = LOAD_PASSENGER_RIGHT;
        }else if(side == -1){
          state = LOAD_PASSENGER_LEFT;
        }
    }
    if(CollisionWatch::hasDetectedCollision()){
        turnAround();
    }
}

void findBeaconLoop(){
    if(ArmAndStepper::isHolding()){
      if(!digitalRead(ARM_SWITCHES[1])){
        ArmAndStepper::setHolding(false);
        state = FIND_PASSENGER;
        ArmAndStepper::drop();
      }
    }
    if(FindBeacon::hasArrived()){
        tapeFollowTest();
        tapeFollowLoop();
        state = DROP_PASSENGER;
    }else{
        tapeFollowLoop();
        CollisionWatch::loop();
        FindBeacon::pickDirection();
        /*Direction dir = getBeaconDirection();
        switch(dir){
        case Direction::LEFT:
            giveTurnDirection(100,0,0.1);
            break;
        case Direction::RIGHT:
            giveTurnDirection(0,100,0.1);
            break;
        default:
            //giveTurnDirection(50,50,50);
            break;
        }*/
        if(CollisionWatch::hasDetectedCollision()){
          turnAround();
        }
    } 
}

void loadPassengerLoop(){
    if(state == LOAD_PASSENGER_RIGHT){
      LCD.clear();
      LCD.print("grabbing on right");
      ArmAndStepper::turnAndReach(true, true);
    }else{
      LCD.clear();
      LCD.print("grabbing on left");
      ArmAndStepper::turnAndReach(false,true);
    }
    
    if(ArmAndStepper::isHolding()){
       state = FIND_BEACON;
       LCD.clear();
       FindBeacon::init();
       PassengerSeek::init();
       tapeFollowStart();
    }else{
       state = FIND_PASSENGER;
       PassengerSeek::init();
       tapeFollowStart();
    }
}

void dropPassengerLoop(){
  Direction dir = FindBeacon::getBeaconDirection();
  if(dir == Direction::LEFT){
    ArmAndStepper::turnAndReach(false,false);
  }else if(dir == Direction::RIGHT){
    ArmAndStepper::turnAndReach(true,false);
  }
  state = FIND_PASSENGER;
  tapeFollowStart();
  PassengerSeek::init();
  FindBeacon::init();
}

void debugSequence(){
  LCD.clear();
  LCD.print("right motor forward");
  LCD.setCursor(0,1);
  LCD.print("START to continue");
  delay(500);
  while(!startbutton()){
    motor.speed(MOTOR_PIN_L, -100);
  }
  motor.speed(MOTOR_PIN_L, 0);
  while(startbutton()){}
  delay(500);
  LCD.clear();
  LCD.print("left motor forward");
  LCD.setCursor(0,1);
  LCD.print("START to continue");
  while(!startbutton()){
    motor.speed(MOTOR_PIN_R, 100);
  }
  motor.speed(MOTOR_PIN_R, 0);
  while(startbutton()){}
  delay(500);
  LCD.clear();
  LCD.print("disengage arm gear");
  LCD.setCursor(0,1);
  LCD.print("START to continue");
  while(!startbutton()){}
  while(startbutton()){}
  delay(500);
  LCD.clear();
  LCD.print("arm gear upward");
  LCD.setCursor(0,1);
  LCD.print("START to continue");
  int printCounter  = 0;
  while(!startbutton()){
    if(printCounter % 50 == 0){
      LCD.clear();
      LCD.print(ArmAndStepper::getAngle());
      printCounter = 0;
    }
    printCounter++;
    ArmAndStepper::doControl();
  }
  motor.speed(MOTOR_PIN_ARM, 0);
  while(startbutton()){}
  delay(500);
  LCD.clear();
  LCD.print("set claw to midpoint");
  LCD.setCursor(0,1);
  LCD.print("START to continue");
  while(!startbutton()){
    LCD.clear();
    LCD.print("STOP to open");
    LCD.setCursor(0,1);
    LCD.print("START to continue");
    delay(50);
    if(stopbutton()){
      delay(500);
      motor.speed(MOTOR_PIN_BABY,-140);
      ArmAndStepper::setHolding(false);
      delay(350/2);
      motor.speed(MOTOR_PIN_BABY,0);
    }
  }
  while(startbutton()){}
  delay(500);
  LCD.clear();
  LCD.print("test claw switches");
  LCD.setCursor(0,1);
  LCD.print("START to continue");
  while(!startbutton()){
    CollisionWatch::loop();
    if(printCounter % 50 == 0){
      LCD.clear();
      if(!digitalRead(ARM_SWITCHES[0])){
        LCD.print("catch switch");
    }else if(!digitalRead(ARM_SWITCHES[1])){
        LCD.print("miss switch");
    }else if(!digitalRead(ARM_SWITCHES[2])){
        LCD.print("detect switch");
    }
      printCounter = 0;
    }
    printCounter++;
  }
  while(startbutton()){}
  delay(500);
  LCD.clear();
  LCD.print("stepper test left");
  LCD.setCursor(0,1);
  LCD.print("START to continue");
  while(!startbutton()){}
  delay(500);
  ArmAndStepper::stepperTurn(false, 200);
  motor.speed(MOTOR_PIN_ARM, 0);
  delay(500);
  LCD.clear();
  LCD.print("stepper test right");
  LCD.setCursor(0,1);
  LCD.print("START to continue");
  while(!startbutton()){}
  delay(500);
  ArmAndStepper::stepperTurn(true,200);
  motor.speed(MOTOR_PIN_ARM, 0);
  delay(500);
  LCD.clear();
  LCD.print("collision sensor test");
  LCD.setCursor(0,1);
  LCD.print("START to continue");
  while(!startbutton()){}
  delay(500);
  while(!startbutton()){
    CollisionWatch::loop();
    if(printCounter % 50 == 0){
      LCD.clear();
      if(CollisionWatch::hasDetectedCollision()){
        LCD.print("COLLISION!");
      }else{
        LCD.print("NO COLLISION");
    }
      printCounter = 0;
    }
    printCounter++;
  }
  while(startbutton()){}
  delay(500);
  LCD.clear();
  LCD.print("QRD test");
  LCD.setCursor(0,1);
  LCD.print("START to continue");
  while(!startbutton()){}
  delay(500);
  tapeFollowInit();
  tapeFollowTest();
  while(!startbutton()){
    tapeFollowLoop();
    if(printCounter % 50 == 0){
      LCD.clear();
      printLCD();
      printCounter = 0;
    }
    printCounter++;
  }
  tapeFollowTest();
  delay(500);
  LCD.clear();
  LCD.print("pass. QSD test");
  LCD.setCursor(0,1);
  LCD.print("START to continue");
  while(!startbutton()){}
  delay(500);
  PassengerSeek::init();
  while(!startbutton()){
    PassengerSeek::loop();
    if(printCounter % 50 == 0){
      LCD.clear();
      PassengerSeek::printLCD();
      printCounter = 0;
    }
    printCounter++;
  }
  PassengerSeek::pause();
  delay(500);
  LCD.clear();
  LCD.print("beacon QSD test");
  LCD.setCursor(0,1);
  LCD.print("START to continue");
  while(!startbutton()){}
  delay(500);
  FindBeacon::init();
  while(!startbutton()){
    FindBeacon::loop();
    if(printCounter % 50 == 0){
      LCD.clear();
      FindBeacon::printLCD();
      printCounter = 0;
    }
    printCounter++;
  }
  delay(500);
  LCD.clear();
  LCD.clear();
  LCD.print("START to begin loop");
  while(!startbutton()){
    delay(10);
  }
  while(startbutton()){}
  delay(500);
  started = true;
}
