///
// main.ino
//
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

void findPassengerLoop();
void loadPassengerLoop();
void findBeaconLoop();
void debugSequence();


void setup() {
    // put your setup code here, to run once:
#include <phys253setup.txt>
    RCServo1.detach();
    RCServo2.detach();
    Serial.begin(9600);
    randomSeed(analogRead(0));
    LCD.clear();
    LCD.print( F("Press START to") );
    LCD.setCursor(0, 1);
    LCD.print( F("begin") );
    
    Arm_And_Stepper::setup();
}

void loop() {
    if (!started && startbutton()) {
        debugSequence();
        TapeFollow::init();
        PassengerSeek::init();
        TapeFollow::start();
        LCD.clear();
    } else if (startbutton()) {
        TapeFollow::start();
        PassengerSeek::init();
        LCD.clear();
        
    } else if (stopbutton()) {
        TapeFollow::test();
        PassengerSeek::pause();
        LCD.clear();
        LCD.print("stopped!");
    }
    if (started) {
        Arm_And_Stepper::doControl(); //Can't not do this or the arm will F*CKING EXPLODE
        if (state == FIND_PASSENGER)
            findPassengerLoop();
        else if (state == LOAD_PASSENGER_LEFT)
            loadPassengerLoop();
        else if (state == LOAD_PASSENGER_RIGHT)
            loadPassengerLoop();
        else if (state == FIND_BEACON)
            findBeaconLoop();
    }
}


void findPassengerLoop()
{
    TapeFollow::loop();
    CollisionWatch::loop();
    PassengerSeek::loop();
    if (PassengerSeek::isAtPassenger()) {
        TapeFollow::test();
        PassengerSeek::pause();
        TapeFollow::loop();
        int side = PassengerSeek::getPassengerSide();
        if (side == 1) 
            state = LOAD_PASSENGER_RIGHT;
        else if (side == -1)
            state = LOAD_PASSENGER_LEFT;
        //PassengerSeek::stop();
        //TODO: uncomment this once passenger seeking is working
    }
    if (CollisionWatch::hasDetectedCollision()) 
        TapeFollow::turnAround();
}

void findBeaconLoop()
{
    if (ToDestination::hasArrived()) {
        TapeFollow::test();
        TapeFollow::loop();
        //LCD.clear();
        //LCD.print("I have arrived");
        //delay(10000);
        //TapeFollow::init();      
        //state = DROP_PASSENGER;
    } else {
        TapeFollow::loop();
        CollisionWatch::loop();
        ToDestination::loop();
        Direction dir = ToDestination::getBeaconDirection();
        switch (dir) {
        case Direction::LEFT:
            TapeFollow::giveTurnDirection(100, 0, 0.1);
            break;
        case Direction::RIGHT:
            TapeFollow::giveTurnDirection(0, 100, 0.1);
            break;
        default:
            TapeFollow::giveTurnDirection(50, 50, 50);
            break;
        }
    } 
}


void loadPassengerLoop()
{
    if (state == LOAD_PASSENGER_RIGHT) {
        LCD.clear();
        LCD.print( F("grabbing on right") );
        Arm_And_Stepper::turnAndReach(true, true);
    } else {
        LCD.clear();
        LCD.print( F("grabbing on left") );
        Arm_And_Stepper::turnAndReach(false, true);
    }
    
    if (/*holding*/true) { //TODO: change back to holding variable!!!! (once we have switches hooked up again)
        //state = FIND_BEACON;
        LCD.clear();
        LCD.print( F("I got something!") );
        delay(5000);
        state = FIND_PASSENGER;
        //TapeFollow::init();
        PassengerSeek::init();
        TapeFollow::start();
    } else {
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


void debugSequence()
{
    LCD.clear();
    LCD.print( F("right motor forward") );
    LCD.setCursor(0, 1);
    LCD.print( F("START to continue") );
    delay(500);
    while (!startbutton()) 
        motor.speed(MOTOR_PIN_L, -100);
    motor.speed(MOTOR_PIN_L, 0);
    while (startbutton()) {}
    delay(500);
    LCD.clear();
    LCD.print( F("left motor forward") );
    LCD.setCursor(0,1);
    LCD.print( F("START to continue") );
    while (!startbutton()) 
        motor.speed(MOTOR_PIN_R, 100);
    motor.speed(MOTOR_PIN_R, 0);
    while (startbutton()) {}
    delay(500);
    LCD.clear();
    LCD.print( F("disengage arm gear") );
    LCD.setCursor(0,1);
    LCD.print( F("START to continue") );
    while (!startbutton()) {}
    while (startbutton()) {}
    delay(500);
    LCD.clear();
    LCD.print( F("arm gear upward") );
    LCD.setCursor(0, 1);
    LCD.print( F("START to continue") );
    while (!startbutton())
        Arm_And_Stepper::doControl();
    motor.speed(MOTOR_PIN_ARM, 0);
    while (startbutton()) {}
    delay(500);
    LCD.clear();
    LCD.print( F("set claw to midpoint") );
    LCD.setCursor(0, 1);
    LCD.print( F("START to continue") );
    while (!startbutton()) {
        LCD.clear();
        LCD.print( F("STOP to open") );
        LCD.setCursor(0, 1);
        LCD.print( F("START to continue") );
        delay(50);
        if (stopbutton()) {
            delay(500);
            Arm_And_Stepper::dropCrap();
        }
    }
    while (startbutton()) {}
    delay(500);
    LCD.clear();
    LCD.print( F("START to begin loop") );
    while (!startbutton()) 
        delay(10);
    while (startbutton()) {}
    delay(500);
    started = true;
}


void missedPassenger()
{
    LCD.clear();
    LCD.print( F("I missed...") );
    delay(5000);
    unsigned long prevTime = millis();
    TapeFollow::init();
    PassengerSeek::init();   
    TapeFollow::start();
    while ((millis() - prevTime) < 750) {
        Arm_And_Stepper::doControl();
        TapeFollow::loop();
    }
    TapeFollow::turnAround();
    state = FIND_PASSENGER;
}
