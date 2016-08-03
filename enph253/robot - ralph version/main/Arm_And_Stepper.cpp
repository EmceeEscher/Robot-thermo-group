///
// Arm_And_Stepper.cpp
//
#include <phys253.h>
#include <phys253pins.h>
#include "pins.hpp"
#include "Arm_And_Stepper.hpp"


extern bool Arm_And_Stepper::holding = false; // Holding a passenger?
extern const int Arm_And_Stepper::dropTime = 350; // Amount of time to run motor to drop animal (ms)

//SPECIFIED GAIN VALUES
const float PROP_GAIN = 12.;
const float HOLD_PROP_GAIN = 12.;
const float INT_GAIN = 0.;
const float DERIV_GAIN = 0.5;

//Dynamic Control Variables
static float baseTarget;
static float midTarget;
static float propErr;
static float derivErr;
static float intErr;
static float lastPropErr;
static float angle;
static float now, lastTime;

//Rest Positions
const float BASE_REST_POSITION = 130;
const float MID_REST_POSITION = 150;
const float BASE_HOLD_POSITION = 135;
const float MID_HOLD_POSITION = 170;//TODO: switch back to 170

//Iterator for LCD printing
static int LCDControl;

//Stepper Constants
const int COUNTERCLOCKWISE = HIGH;
const int CLOCKWISE = LOW;
const int stepperMicrosDelay = 1200; //Time delay between pulses in microseconds
const int numPulses = 680;

//reachAndGrab/reachAndDrop function Constants
const float initialAdjMidTarget = 100;
const float initialAdjBaseTarget = 120;
const float midAdjMidTarget = 60;
const float midAdjBaseTarget = 120;
const float finalAdjMidTarget = 10;
const float finalAdjBaseTarget = 115;
const float midGrabTarget = 60;
const float baseGrabTarget = 100;


namespace Arm_And_Stepper
{

    static void setBaseMotor(long duty);
    static float getControlValue();
    static void grabCrap();
    static void reachAndClaw(bool grabbing);
    static void setRestPosition();
    static void refinedReachAndGrab();
    
}


void Arm_And_Stepper::setup()
{
    //Arm & Stepper Initialization code
    pinMode(pins::DIR_PIN, OUTPUT);
    pinMode(pins::PULSE_PIN, OUTPUT);
    baseTarget = BASE_REST_POSITION;
    midTarget = MID_REST_POSITION;
    lastPropErr = 0.;
}


// The control loop. Should be implemented wherever the code
//be for extended perids of time to prevent arm from overcorrecting,
//grinding gears, etc.
// Should be repeated with several millisecond delays for integral/
//derivative control purposes
void Arm_And_Stepper::doControl()
{
    angle = getAngle();
    
    now = micros();
    propErr = angle - baseTarget;
    derivErr = (propErr - lastPropErr) / (now - lastTime) * 1000000.;
    intErr += (propErr + lastPropErr) / 2 * (now - lastTime) / 1000000.;
    setBaseMotor(static_cast<long>(getControlValue()));
    
    RCServo0.write(midTarget);
    
    lastTime = now;
    lastPropErr = propErr;
    
    if (LCDControl % 25 == 0) {
        //printState();
        LCDControl = 1;
    }
    ++LCDControl;
}


//Converts base potentiometer voltage to corresponding angle
float Arm_And_Stepper::getAngle()
{
    float voltage = static_cast<float>(analogRead(pins::POTENTIOMETER)) *
        5. / 1024.;
    return 130.814 * (3. * voltage - 10.) / (voltage - 5.) + 60.;
}


//Wrapper function for setting motor speed
//Prevents values larger than 255 in either direction
void Arm_And_Stepper::setBaseMotor(long duty)
{
    if (duty > 255) 
        duty = 255;
    else if (duty < -255) 
        duty = -255;
    motor.speed(pins::MOTOR_PIN_ARM, duty);
}


//Returns the motor speed based on PID control
float Arm_And_Stepper::getControlValue()
{
    float control = 0;
    if (holding) 
        control += HOLD_PROP_GAIN * propErr;
    else
        control += PROP_GAIN * propErr;
    control += DERIV_GAIN * derivErr;
    control += INT_GAIN * intErr;
    return control;
}


//Closes the claw until something is detected in claw, the claw
//closes on itself or a timeout is reached
void Arm_And_Stepper::grabCrap()
{
    //If switches are already triggered, then do nothing
    if (digitalRead(pins::ARM_SWITCHES[0]) &&
        digitalRead(pins::ARM_SWITCHES[1])) {
        motor.speed(pins::MOTOR_PIN_BABY,190);
        unsigned long startTime = millis();
        while (true) {
            Arm_And_Stepper::doControl();
            if (!digitalRead(pins::ARM_SWITCHES[0])) {
                holding = true;
                break;
            } else if (!digitalRead(pins::ARM_SWITCHES[1])) {
                holding = false;
                break;
            }
            if (millis() - startTime > 2000) {
                holding = true;
                break;
            }
        }
        if (holding) 
            motor.speed(pins::MOTOR_PIN_BABY, 140);
        else
            Arm_And_Stepper::dropCrap();
    }
}


//Opens the claw for specified time
void Arm_And_Stepper::dropCrap()
{
    motor.speed(pins::MOTOR_PIN_BABY, -140);
    holding = false;
    delay(dropTime);
    motor.speed(pins::MOTOR_PIN_BABY, 0);
}


void Arm_And_Stepper::reachAndClaw(bool grabbing)
{
    midTarget = initialAdjMidTarget;
    baseTarget = initialAdjBaseTarget;
    unsigned long startTime = millis();
    while (millis() - startTime < 500) 
        Arm_And_Stepper::doControl();
    
    midTarget = midAdjMidTarget;
    baseTarget = midAdjBaseTarget;
    
    while (millis() - startTime < 1000) 
        Arm_And_Stepper::doControl();
    
    baseTarget = finalAdjBaseTarget; 
    midTarget = finalAdjMidTarget;
    while (millis() - startTime < 1500) {  
        Arm_And_Stepper::doControl();
        if (!digitalRead(pins::ARM_SWITCHES[2])) {
            baseTarget = getAngle() + 5;
            break;
        } 
    }
    if (grabbing) 
        grabCrap();
    else 
        dropCrap();
    setRestPosition();
}


void Arm_And_Stepper::setRestPosition()
{
    unsigned long startTime = millis();
    baseTarget = midAdjBaseTarget; 
    midTarget = midAdjMidTarget;
    while (millis() - startTime < 500) 
        Arm_And_Stepper::doControl();
    
    baseTarget = initialAdjBaseTarget;
    midTarget = initialAdjMidTarget;
    while (millis() - startTime < 1000)  // TODO: no hard coding
        Arm_And_Stepper::doControl();
    
    if (holding) {
        baseTarget = BASE_HOLD_POSITION;
        midTarget = MID_HOLD_POSITION;
    } else {
        baseTarget = BASE_REST_POSITION;
        midTarget = MID_REST_POSITION;
    }
}


//Turns the stepper motor a specified number of steps
void Arm_And_Stepper::stepperTurn(bool CW,int count){
    if (CW)
        digitalWrite(pins::DIR_PIN, CLOCKWISE);
    else
        digitalWrite(pins::DIR_PIN, COUNTERCLOCKWISE);
    
    unsigned long prevTime = millis();
    for (int i = 0; i < count; ++i) {
        if (millis() - prevTime > 10) {
            Arm_And_Stepper::doControl();
            prevTime = millis();
        }
        digitalWrite(pins::PULSE_PIN, HIGH);
        delayMicroseconds(stepperMicrosDelay);
        
        digitalWrite(pins::PULSE_PIN, LOW);
        delayMicroseconds(stepperMicrosDelay);
    }
}


/* 
 * Parameter: turnRight - Turn right if true, left otherwise
 * Parameter: grab - grab if true, drop otherwise
 */
void Arm_And_Stepper::turnAndReach(bool turnRight, bool grab)
{
    LCD.clear();
    LCD.print( F("initial turn") );
    Arm_And_Stepper::stepperTurn(turnRight, numPulses);
    LCD.clear();
    LCD.print( F("grabbing in turn and reach") );
    if (grab)
        Arm_And_Stepper::refinedReachAndGrab();
    else
        Arm_And_Stepper::reachAndClaw(grab);
    LCD.clear();
    LCD.print( F("turning back") );
    Arm_And_Stepper::stepperTurn(!turnRight, numPulses);
}


void Arm_And_Stepper::refinedReachAndGrab(){
    midTarget = midGrabTarget;
    baseTarget = baseGrabTarget;
    unsigned long startTime = millis();
    while (true) {
        doControl();
        if ((millis() - startTime) > 250) {
            if (baseTarget > 100) 
                baseTarget -= 5;
            else if (midTarget < 160 && baseTarget <= 100)
                midTarget += 10;
            startTime = millis();
        }
        if (baseTarget <= 100 && midTarget >= 160)
            break;
        delay(5);
    }
    grabCrap();
    setRestPosition();
}
