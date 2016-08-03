///
// Arm_And_Stepper.cpp
//
#include <phys253.h>
#include <phys253pins.h>
#include "pins.hpp"
#include "Arm_And_Stepper.hpp"


//Amount of time to run motor to drop animal
extern const int Arm_And_Stepper::dropTime = 400; // Milliseconds

//SPECIFIED GAIN VALUES
const float PROP_GAIN = 10.;
const float HOLD_PROP_GAIN = 10.;
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
// static float currAngle = 0;
static float now, lastTime;
// static int hasInitialized = 0;

//Rest Positions
const float BASE_REST_POSITION = 115;
const float MID_REST_POSITION  = 150;
const float BASE_HOLD_POSITION = 115;
const float MID_HOLD_POSITION  = 150;//TODO: switch back to 170

//Iterator for LCD printing
static int LCDControl;

//Stepper Constants
const int COUNTERCLOCKWISE = HIGH;
const int CLOCKWISE = LOW;
const int stepperMicrosDelay = 1200; //Time delay between pulses in microseconds
const int numPulses = 680;

//reachAndGrab/reachAndDrop function Constants
const float initialAdjMidTarget  = 120;
const float initialAdjBaseTarget = 120;
const float midAdjMidTarget      = 60;
const float midAdjBaseTarget     = 115;
const float finalAdjMidTarget    = 60;
const float finalAdjBaseTarget   = 100;

//Holding a passenger?
bool Arm_And_Stepper::holding = false;


namespace Arm_And_Stepper
{

    static void setBaseMotor(long duty);
    static float getControlValue();
    static void grabCrap();
    static void reachAndClaw(bool grabbing);
    static void setRestPosition();
    
}


void Arm_And_Stepper::setup()
{
    //Arm & Stepper Initialization code
    pinMode(DIR_PIN,OUTPUT);
    pinMode(PULSE_PIN,OUTPUT);
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
    float voltage = static_cast<float>(analogRead(POTENTIOMETER)) * 5. / 1024.;
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
    motor.speed(MOTOR_PIN_ARM, duty);
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
    if (digitalRead(ARM_SWITCHES[0]) && digitalRead(ARM_SWITCHES[1])) {
        motor.speed(MOTOR_PIN_BABY, 190);
        unsigned long startTime = millis();
        while (true) {
            Arm_And_Stepper::doControl();
            if (!digitalRead(ARM_SWITCHES[0])) {
                holding = true;
                break;
            } else if (!digitalRead(ARM_SWITCHES[1])) {
                holding = false;
                break;
            } else if (millis() - startTime > 2000) {
                holding = true;
                break;
            }
        }
        if (holding) 
            motor.speed(MOTOR_PIN_BABY, 20);
        else
            dropCrap();
    }
}

//Opens the claw for specified time
void Arm_And_Stepper::dropCrap()
{
    motor.speed(MOTOR_PIN_BABY, -140);
    holding = false;
    delay(dropTime);
    motor.speed(MOTOR_PIN_BABY, 0);
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
        if (!digitalRead(ARM_SWITCHES[2])) {
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
        digitalWrite(DIR_PIN, CLOCKWISE);
    else
        digitalWrite(DIR_PIN, COUNTERCLOCKWISE);
    
    unsigned long prevTime = millis();
    for (int i = 0; i < count; ++i) {
        if (millis() - prevTime > 10) {
            Arm_And_Stepper::doControl();
            prevTime = millis();
        }
        digitalWrite(PULSE_PIN, HIGH);
        delayMicroseconds(stepperMicrosDelay);
        
        digitalWrite(PULSE_PIN, LOW);
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
    stepperTurn(turnRight, numPulses);
    LCD.clear();
    LCD.print( F("grabbing in turn and reach") );
    reachAndClaw(grab);
    LCD.clear();
    LCD.print( F("turning back") );
    stepperTurn(!turnRight, numPulses);
}

