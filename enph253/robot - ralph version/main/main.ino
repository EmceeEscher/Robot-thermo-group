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

static bool started = false;
static int state = FIND_PASSENGER;
static int printCount = 0;

void findBeaconLoop();
void dropPassengerLoop();
void findPassengerLoop();
void loadPassengerLoop();
void debugSequence();

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
    Arm_And_Stepper::setup();
}

void loop() {
    if(!started && startbutton()){
        //debugSequence();
        started = true;
        TapeFollow::init();
        PassengerSeek::init();
        TapeFollow::start();
        ToDestination::init();
        LCD.clear();
    }
    if(!started && stopbutton()){
        debugSequence();
        TapeFollow::init();
        PassengerSeek::init();
        TapeFollow::start();
        ToDestination::init();
        LCD.clear();
    }
    if(startbutton() && started){
        TapeFollow::start();
        PassengerSeek::init();
        LCD.clear();
        
    }
    if(stopbutton() && started){
        TapeFollow::test();
        PassengerSeek::pause();
        LCD.clear();
        LCD.print("stopped!");
    }
    
    if (started && printCount % PRINT_PERIOD == 0) {
        
        if(state == FIND_PASSENGER){
            PassengerSeek::printLCD();
            //PassengerSeek::printLCD();
        }else if(state == FIND_BEACON)
            ToDestination::printLCD();
        //printLCD();
        printCount = 0;
    }
    ++printCount;
    if(started){
        Arm_And_Stepper::doControl(); //Can't not do this or the arm will FUCKING EXPLODE
        
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
    TapeFollow::loop();
    CollisionWatch::loop();
    PassengerSeek::loop();
    if(PassengerSeek::isAtPassenger()){
        TapeFollow::test();
        PassengerSeek::pause();
        TapeFollow::loop();
        int side = PassengerSeek::getPassengerSide();
        if(side == 1){
            state = LOAD_PASSENGER_RIGHT;
        }else if(side == -1){
            state = LOAD_PASSENGER_LEFT;
        }
        //state = FIND_BEACON;
        //TapeFollow::start();
        PassengerSeek::init();
        //PassengerSeek::stop();
        //TODO: uncomment this once passenger seeking is working
    }
    if(CollisionWatch::hasDetectedCollision()){
        TapeFollow::turnAround();
    }
}

void findBeaconLoop(){
    if(Arm_And_Stepper::holding){
        if(!digitalRead(pins::ARM_SWITCHES[1])){
            Arm_And_Stepper::holding = false;
            state = FIND_PASSENGER;
            Arm_And_Stepper::dropCrap();
        }
    }
    if(ToDestination::hasArrived()){
        TapeFollow::test();
        TapeFollow::loop();
        state = DROP_PASSENGER;
        //LCD.clear();
        //LCD.print("I have arrived");
        //delay(10000);
        //TapeFollow::init();      
        //state = DROP_PASSENGER;
    }else{
        TapeFollow::loop();
        CollisionWatch::loop();
        ToDestination::loop();
        Direction dir = ToDestination::getBeaconDirection();
        switch(dir){
        case Direction::LEFT:
            TapeFollow::giveTurnDirection(100,0,0.1);
            break;
        case Direction::RIGHT:
            TapeFollow::giveTurnDirection(0,100,0.1);
            break;
        default:
            TapeFollow::giveTurnDirection(50,50,50);
            break;
        }
        if(CollisionWatch::hasDetectedCollision()){
            TapeFollow::turnAround();
        }
    } 
}

void loadPassengerLoop(){
    if(state == LOAD_PASSENGER_RIGHT){
        LCD.clear();
        LCD.print("grabbing on right");
        Arm_And_Stepper::turnAndReach(true, true);
    }else{
        LCD.clear();
        LCD.print("grabbing on left");
        Arm_And_Stepper::turnAndReach(false,true);
    }
    
    if(Arm_And_Stepper::holding){ //TODO: change back to Arm_And_Stepper::holding variable!!!! (once we have switches hooked up again)
        state = FIND_BEACON;
        LCD.clear();
        //LCD.print("I got something!");
        //2state = FIND_PASSENGER;
        //TapeFollow::init();
        PassengerSeek::init();
        TapeFollow::start();
    }else{
        //go forward
        //turn around
        //find passenger again 
        state = FIND_PASSENGER;
        //TapeFollow::init();
        PassengerSeek::init();
        TapeFollow::start();
        //missedPassenger();//TODO: TEST THIS!!!!
    }
}

void dropPassengerLoop(){
    Direction dir = ToDestination::getBeaconDirection();
    if(dir == Direction::LEFT){
        Arm_And_Stepper::turnAndReach(false,false);
    }else if(dir == Direction::RIGHT){
        Arm_And_Stepper::turnAndReach(true,false);
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
        motor.speed(pins::MOTOR_PIN_L, -100);
    }
    motor.speed(pins::MOTOR_PIN_L, 0);
    while(startbutton()){}
    delay(500);
    LCD.clear();
    LCD.print("left motor forward");
    LCD.setCursor(0,1);
    LCD.print("START to continue");
    while(!startbutton()){
        motor.speed(pins::MOTOR_PIN_R, 100);
    }
    motor.speed(pins::MOTOR_PIN_R, 0);
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
            LCD.print(Arm_And_Stepper::getAngle());
            printCounter = 0;
        }
        printCounter++;
        Arm_And_Stepper::doControl();
    }
    motor.speed(pins::MOTOR_PIN_ARM, 0);
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
            motor.speed(pins::MOTOR_PIN_BABY,-140);
            Arm_And_Stepper::holding = false;
            delay(Arm_And_Stepper::dropTime/2);
            motor.speed(pins::MOTOR_PIN_BABY,0);
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
            if(!digitalRead(pins::ARM_SWITCHES[0])){
                LCD.print("catch switch");
            }else if(!digitalRead(pins::ARM_SWITCHES[1])){
                LCD.print("miss switch");
            }else if(!digitalRead(pins::ARM_SWITCHES[2])){
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
    Arm_And_Stepper::stepperTurn(false, 200);
    motor.speed(pins::MOTOR_PIN_ARM, 0);
    delay(500);
    LCD.clear();
    LCD.print("stepper test right");
    LCD.setCursor(0,1);
    LCD.print("START to continue");
    while(!startbutton()){}
    delay(500);
    Arm_And_Stepper::stepperTurn(true,200);
    motor.speed(pins::MOTOR_PIN_ARM, 0);
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
    TapeFollow::init();
    TapeFollow::test();
    while(!startbutton()){
        TapeFollow::loop();
        if(printCounter % 50 == 0){
            LCD.clear();
            TapeFollow::printLCD();
            printCounter = 0;
        }
        printCounter++;
    }
    TapeFollow::test();
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
    ToDestination::init();
    while(!startbutton()){
        ToDestination::loop();
        if(printCounter % 50 == 0){
            LCD.clear();
            ToDestination::printLCD();
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
    TapeFollow::init();
    PassengerSeek::init();   
    TapeFollow::start();
    while((millis() - prevTime) < 750){
        Arm_And_Stepper::doControl();
        TapeFollow::loop();
    }
    TapeFollow::turnAround();
    state = FIND_PASSENGER;
}
