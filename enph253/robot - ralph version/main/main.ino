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
  if(!started && startbutton()){
    debugSequence();
    tapeFollowInit();
    PassengerSeek::init();
    tapeFollowStart();
    LCD.clear();
  }
  if(startbutton()){
      tapeFollowStart();
      PassengerSeek::init();
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
            loadPassengerLoop();
        } else if(state == LOAD_PASSENGER_RIGHT){
            loadPassengerLoop();
        } else if(state == FIND_BEACON){
            findBeaconLoop();
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
        //PassengerSeek::stop();
        //TODO: uncomment this once passenger seeking is working
    }
    if(hasDetectedCollision()){
        turnAround();
    }
}

void findBeaconLoop(){
    if(hasArrived()){
        tapeFollowTest();
        tapeFollowLoop();
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
    
    if(/*holding*/true){ //TODO: change back to holding variable!!!! (once we have switches hooked up again)
       //state = FIND_BEACON;
       LCD.clear();
       LCD.print("I got something!");
       delay(5000);
       state = FIND_PASSENGER;
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
  while(!startbutton()){
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
      dropShit();
    }
  }
  while(startbutton()){}
  delay(500);
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
    delay(5000);
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
