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

const int PRINT_PERIOD {200};

static bool started = false;
static int state = FIND_PASSENGER;
static int printCount = 0;

void findPassengerLoop();
void loadPassengerLoop();
void findBeaconLoop();
void dropPassengerLoop();
void debugSequence();


void setup() {
    // put your setup code here, to run once:
#include <phys253setup.txt>
    RCServo1.detach();
    RCServo2.detach();
    Serial.begin(9600);
    randomSeed(analogRead(0));
    LCD.clear();
    LCD.print( F("Bruhh...") );
    delay(250);
    LCD.print( F("MURPH IS") );
    LCD.setCursor(0, 1);
    LCD.print( F("DRURPH  ") );
    delay(500);
    LCD.clear();
    LCD.print( F("START: compete") );
    LCD.setCursor(0, 1);
    LCD.print( F("STOP: debug") );
    
    for (auto i(0); i < 16; ++i) {
        pinMode(i, INPUT_PULLUP);
    }

    //Arm & Stepper Initialization code
    Arm_And_Stepper::setup();
}


void loop() {
    if (!started && startbutton()) {
        //debugSequence();
        started = true;
        TapeFollow::init();
        PassengerSeek::init();
        TapeFollow::start();
        ToDestination::init();
        LCD.clear();
    } else if (!started && stopbutton()) {
        debugSequence();
        TapeFollow::init();
        PassengerSeek::init();
        TapeFollow::start();
        ToDestination::init();
        LCD.clear();
    } else if (startbutton()) {
        TapeFollow::start();
        PassengerSeek::init();
        LCD.clear();
    } else if (stopbutton()) {
        TapeFollow::test();
        PassengerSeek::pause();
        LCD.clear();
        LCD.print( F("stopped!") );
    }

    if (started && printCount % PRINT_PERIOD == 0) {
        if (state == FIND_PASSENGER)
            PassengerSeek::printLCD();
        else if(state == FIND_BEACON)
            ToDestination::printLCD();
        printCount = 0;
    }
    ++printCount;
    if (started) {
        Arm_And_Stepper::doControl(); //Can't not do this or the arm will FUCKING EXPLODE
        if (state == FIND_PASSENGER) 
            findPassengerLoop();
        else if (state == LOAD_PASSENGER_LEFT)
            loadPassengerLoop();
        else if (state == LOAD_PASSENGER_RIGHT)
            loadPassengerLoop();
        else if (state == FIND_BEACON)
            findBeaconLoop();
        else if(state == DROP_PASSENGER)
            dropPassengerLoop();
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
        PassengerSeek::init();
        //PassengerSeek::stop();
        //TODO: uncomment this once passenger seeking is working
    }
    if (CollisionWatch::hasDetectedCollision()) 
        TapeFollow::turnAround();
}


void findBeaconLoop()
{
    if (Arm_And_Stepper::holding) {
        if (!digitalRead(pins::ARM_SWITCHES[1])) {
            Arm_And_Stepper::holding = false;
            state = FIND_PASSENGER;
            Arm_And_Stepper::dropCrap();
        }
    }
    if (ToDestination::hasArrived()) {
        TapeFollow::test();
        TapeFollow::loop();
        state = DROP_PASSENGER;
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
        if (CollisionWatch::hasDetectedCollision()) {
            TapeFollow::turnAround();
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
    
    if (Arm_And_Stepper::holding) { 
        state = FIND_BEACON;
        LCD.clear();
        PassengerSeek::init();
        TapeFollow::start();
    } else {
       //go forward
       //turn around
       //find passenger again 
       state = FIND_PASSENGER;
       PassengerSeek::init();
       TapeFollow::start();
    }
}


void dropPassengerLoop()
{
    Direction dir = ToDestination::getBeaconDirection();
    if (dir == Direction::LEFT) 
        Arm_And_Stepper::turnAndReach(false,false);
    else if(dir == Direction::RIGHT)
        Arm_And_Stepper::turnAndReach(true,false);
    state = FIND_PASSENGER;
}


void debugSequence()
{
    LCD.clear();
    LCD.print( F("right motor forward") );
    LCD.setCursor(0, 1);
    LCD.print( F("START to continue") );
    delay(500);
    while (!startbutton())
        motor.speed(pins::MOTOR_PIN_L, -100);
    motor.speed(pins::MOTOR_PIN_L, 0);
    while (startbutton()) {}
    delay(500);
    LCD.clear();
    LCD.print( F("left motor forward") );
    LCD.setCursor(0, 1);
    LCD.print( F("START to continue") );
    while (!startbutton()) 
        motor.speed(pins::MOTOR_PIN_R, 100);
    motor.speed(pins::MOTOR_PIN_R, 0);
    while (startbutton()) {}
    delay(500);
    LCD.clear();
    LCD.print( F("disengage arm gear") );
    LCD.setCursor(0, 1);
    LCD.print( F("START to continue") );
    while (!startbutton()) {}
    while (startbutton()) {}
    delay(500);
    LCD.clear();
    LCD.print( F("arm gear upward") );
    LCD.setCursor(0, 1);
    LCD.print( F("START to continue") );
    int printCounter = 0;
    while (!startbutton()) {}
    if (printCounter % 50 == 0) {
        LCD.clear();
        LCD.print(Arm_And_Stepper::getAngle());
        printCounter = 0;
    }
    motor.speed(pins::MOTOR_PIN_ARM, 0);
    while (startbutton()) {}
    delay(500);
    LCD.clear();
    LCD.print( F("set claw to midpoint") );
    LCD.setCursor(0, 1);
    LCD.print( F("START to continue") );
    while (!startbutton()) {
        LCD.clear();
        LCD.print( F("left motor forward") );
        LCD.setCursor(0, 1);
        LCD.print( F("START to continue") );
        delay(50);
        if (stopbutton()) {
            delay(500);
            motor.speed(pins::MOTOR_PIN_BABY,-140);
            Arm_And_Stepper::holding = false;
            delay(Arm_And_Stepper::dropTime/2);
            motor.speed(pins::MOTOR_PIN_BABY,0);
        }
    }
    while (startbutton()) {}
    delay(500);
    LCD.clear();
    LCD.print( F("stepper test left") );
    LCD.setCursor(0, 1);
    LCD.print( F("START to continue") );
    while (!startbutton()) {}
    delay(500);
    Arm_And_Stepper::stepperTurn(false, 200);
    motor.speed(pins::MOTOR_PIN_ARM, 0);
    delay(500);
    LCD.clear();
    LCD.print( F("stepper test right") );
    LCD.setCursor(0, 1);
    LCD.print( F("START to continue") );
    while (!startbutton()) {}
    delay(500);
    Arm_And_Stepper::stepperTurn(true, 200);
    motor.speed(pins::MOTOR_PIN_ARM, 0);
    delay(500);
    LCD.clear();
    LCD.print( F("START to begin loop") );
    while (!startbutton()) {
        delay(10);
    }
    while (startbutton()) {}
    delay(500);
    LCD.clear();
    LCD.print( F("test claw switches") );
    LCD.setCursor(0, 1);
    LCD.print( F("START to continue") );
    while (!startbutton()) {
        CollisionWatch::loop();
        if (printCounter % 50 == 0) {
            LCD.clear();
            if (!digitalRead(pins::ARM_SWITCHES[0])) 
                LCD.print( F("catch switch") );
            else if (!digitalRead(pins::ARM_SWITCHES[1])) 
                LCD.print( F("miss switch") );
            else if (!digitalRead(pins::ARM_SWITCHES[2])) 
                LCD.print( F("detect switch") );
            printCounter = 0;
        }
        ++printCounter;
    }
    while (startbutton()) {}
    delay(500);
    LCD.clear();
    LCD.print( F("stepper test left") );
    LCD.setCursor(0, 1);
    LCD.print( F("START to continue") );
    while (!startbutton()) {}
    delay(500);
    Arm_And_Stepper::stepperTurn(false, 200);
    motor.speed(pins::MOTOR_PIN_ARM, 0);
    delay(500);
    LCD.clear();
    LCD.print( F("stepper test right") );
    LCD.setCursor(0, 1);
    LCD.print( F("START to continue") );
    while (!startbutton()) {}
    delay(500);
    Arm_And_Stepper::stepperTurn(true, 200);
    motor.speed(pins::MOTOR_PIN_ARM, 0);
    delay(500);
    LCD.clear();
    LCD.print( F("collision sensor test") );
    LCD.setCursor(0, 1);
    LCD.print( F("START to continue") );
    while (!startbutton()) {}
    delay(500);
    while (!startbutton()) {
        CollisionWatch::loop();
        if (printCounter % 50 == 0) {
            LCD.clear();
            if (CollisionWatch::hasDetectedCollision()) 
                LCD.print( F("COLLISION!") );
            else
                LCD.print( F("NO COLLISION") );
            printCounter = 0;
        }
        ++printCounter;
    }
    while (startbutton()) {}
    delay(500);
    LCD.clear();
    LCD.print( F("QRD test") );
    LCD.setCursor(0, 1);
    LCD.print( F("START to continue") );
    while (!startbutton()) {}
    delay(500);
    TapeFollow::init();
    TapeFollow::test();
    while (!startbutton()) {
        TapeFollow::loop();
        if (printCounter % 50 == 0) {
            LCD.clear();
            TapeFollow::printLCD();
            printCounter = 0;
        }
        ++printCounter;
    }
    TapeFollow::test();
    delay(500);
    LCD.clear();
    LCD.print( F("pass. QSD test") );
    LCD.setCursor(0, 1);
    LCD.print( F("START to continue") );
    while (!startbutton()) {}
    delay(500);
    PassengerSeek::init();
    while (!startbutton()) {
        PassengerSeek::loop();
        if (printCounter % 50 == 0) {
            LCD.clear();
            PassengerSeek::printLCD();
            printCounter = 0;
        }
        ++printCounter;
    }
    PassengerSeek::pause();
    delay(500);
    LCD.clear();
    LCD.print( F("beacon QSD test") );
    LCD.setCursor(0, 1);
    LCD.print( F("START to continue") );
    while (!startbutton()) {}
    delay(500);
    ToDestination::init();
    while (!startbutton()) {
        ToDestination::loop();
        if (printCounter % 50 == 0) {
            LCD.clear();
            ToDestination::printLCD();
            printCounter = 0;
        }
        ++printCounter;
    }
    PassengerSeek::pause();
    delay(500);
    LCD.clear();
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
