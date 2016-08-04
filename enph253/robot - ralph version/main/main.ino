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
//    for(auto i(40); i < 48, i++){
//      pinMode(i, INPUT);
//    }
    //Arm & Stepper Initialization code
    pinMode(DIR_PIN,OUTPUT);
    pinMode(PULSE_PIN,OUTPUT);
    baseTarget = BASE_REST_POSITION;
    midTarget = MID_REST_POSITION;
    lastPropErr = 0.;
}

void loop() {
  if(!started && startbutton()){
    //debugSequence();
    started = true;
    tapeFollowInit();
    PassengerSeek::init();
    tapeFollowStart();
    beaconInit();
    LCD.clear();
  }
  if(!started && stopbutton()){
    debugSequence();
    tapeFollowInit();
    PassengerSeek::init();
    tapeFollowStart();
    beaconInit();
    LCD.clear();
  }
  if(startbutton() && started){
      tapeFollowStart();
      PassengerSeek::init();
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
          //printLCD();
          PassengerSeek::printLCD();
        }else if(state == FIND_BEACON)
          detectBeaconPrintLCD();
          //printLCD();
        printCount = 0;
    }
    ++printCount;
  if(started){
        doControl(); //Can't not do this or the arm will FUCKING EXPLODE
        
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
    collisionLoop();
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
        //state = FIND_BEACON;
        //tapeFollowStart();
        //PassengerSeek::init();
        //PassengerSeek::stop();
        //TODO: uncomment this once passenger seeking is working
    }
    if(hasDetectedCollision()){
        turnAround();
    }
}

void findBeaconLoop(){
    if(holding){
      if(!digitalRead(ARM_SWITCHES[1])){
        holding = false;
        state = FIND_PASSENGER;
        dropShit();
      }
    }
    if(hasArrived()){
        tapeFollowTest();
        tapeFollowLoop();
        state = DROP_PASSENGER;
        //LCD.clear();
        //LCD.print("I have arrived");
        //delay(10000);
        //tapeFollowInit();      
        //state = DROP_PASSENGER;
    }else{
        tapeFollowLoop();
        collisionLoop();
        detectBeaconLoop();
        Direction dir = getBeaconDirection();
        switch(dir){
        case Direction::LEFT:
            giveTurnDirection(100,0,0.1);
            break;
        case Direction::RIGHT:
            giveTurnDirection(0,100,0.1);
            break;
        default:
            giveTurnDirection(50,50,50);
            break;
        }
        if(hasDetectedCollision()){
          turnAround();
        }
    } 
}

void loadPassengerLoop(){
    if(state == LOAD_PASSENGER_RIGHT){
      LCD.clear();
      LCD.print("grabbing on right");
      turnAndReach(true, true);
    }else{
      LCD.clear();
      LCD.print("grabbing on left");
      turnAndReach(false,true);
    }
    
    if(holding){ //TODO: change back to holding variable!!!! (once we have switches hooked up again)
       state = FIND_BEACON;
       LCD.clear();
       //LCD.print("I got something!");
       //2state = FIND_PASSENGER;
       //tapeFollowInit();
       PassengerSeek::init();
       tapeFollowStart();
    }else{
       //go forward
       //turn around
       //find passenger again 
       state = FIND_PASSENGER;
       //tapeFollowInit();
       PassengerSeek::init();
       tapeFollowStart();
       //missedPassenger();//TODO: TEST THIS!!!!
    }
}

void dropPassengerLoop(){
  Direction dir = getBeaconDirection();
  if(dir == Direction::LEFT){
    turnAndReach(false,false);
  }else if(dir == Direction::RIGHT){
    turnAndReach(true,false);
  }
  state = FIND_PASSENGER;
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
      LCD.print(getAngle());
      printCounter = 0;
    }
    printCounter++;
    doControl();
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
      holding = false;
      delay(dropTime/2);
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
    collisionLoop();
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
  stepperTurn(false, 200);
  motor.speed(MOTOR_PIN_ARM, 0);
  delay(500);
  LCD.clear();
  LCD.print("stepper test right");
  LCD.setCursor(0,1);
  LCD.print("START to continue");
  while(!startbutton()){}
  delay(500);
  stepperTurn(true,200);
  motor.speed(MOTOR_PIN_ARM, 0);
  delay(500);
  LCD.clear();
  LCD.print("collision sensor test");
  LCD.setCursor(0,1);
  LCD.print("START to continue");
  while(!startbutton()){}
  delay(500);
  while(!startbutton()){
    collisionLoop();
    if(printCounter % 50 == 0){
      LCD.clear();
      if(hasDetectedCollision()){
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
  beaconInit();
  while(!startbutton()){
    detectBeaconLoop();
    if(printCounter % 50 == 0){
      LCD.clear();
      detectBeaconPrintLCD();
      printCounter = 0;
    }
    printCounter++;
  }
  PassengerSeek::pause();
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

void missedPassenger(){
    LCD.clear();
    LCD.print("I missed...");
    unsigned long prevTime = millis();
    tapeFollowInit();
    PassengerSeek::init();   
    tapeFollowStart();
    while((millis() - prevTime) < 750){
      doControl();
      tapeFollowLoop();
    }
    turnAround();
    state = FIND_PASSENGER;
}
