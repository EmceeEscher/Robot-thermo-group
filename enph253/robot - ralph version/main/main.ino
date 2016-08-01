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
            if(holding){
                LCD.clear();
                LCD.print("I got something!");
                delay(5000);
                state = FIND_PASSENGER;
                //state = FIND_BEACON;
            }else{
                //go forward
                //turn around
                //find passenger again 
                missedPassenger();//TODO: TEST THIS!!!!
            }
        } else if(state == LOAD_PASSENGER_RIGHT){
            turnAndReach(true, true);
            if(holding){
                //state = FIND_BEACON;
                LCD.clear();
                LCD.print("I got something!");
                delay(5000);
                state = FIND_PASSENGER;
            }else{
                //go forward
                //turn around
                //find passenger again 
                missedPassenger();//TODO: TEST THIS!!!!
            }
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

void findBeaconLoop(){
    if(hasArrived()){
        state = DROP_PASSENGER;
    }else{
        tapeFollowLoop();
        collisionLoop();
        Direction dir = getBeaconDirection();
        switch(dir){
        case Direction::LEFT:
            giveTurnDirection(100,0,0.1);
            break;
        case Direction::RIGHT:
            giveTurnDirection(0,100,0.1);
            break;
        default:
            giveTurnDirection(0.1,0.1,100);
            break;
        }
    } 
}

void missedPassenger(){
    /*for(auto i(0); i < 30; i++){
      tapeFollowLoop();
      }
      turnAround();*/
    LCD.clear();
    LCD.print("I missed...");
    delay(5000);
    state = FIND_PASSENGER;
}
