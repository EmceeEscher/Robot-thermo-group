#include <phys253.h>
#include <phys253pins.h>
#include "ArmAndStepper.hpp"
#include "pins.hpp"


//Amount of time to run motor to drop animal
const int dropTime = 350; // Milliseconds

//SPECIFIED GAIN VALUES
const float PROP_GAIN = 12.;
const float HOLD_PROP_GAIN = 12.;
const float INT_GAIN = 0.;
const float DERIV_GAIN = 0.5;

//Dynamic Control Variables
static float baseTarget, midTarget;
static float propErr, derivErr, intErr, lastPropErr;
static float angle;
static float currAngle = 0;
static float now, lastTime;
static int hasInitialized = 0;

//Rest Positions
const float BASE_REST_POSITION = 125;
const float MID_REST_POSITION = 165;
const float BASE_HOLD_POSITION = 125;
const float MID_HOLD_POSITION = 170;

//Stepper Constants
const int COUNTERCLOCKWISE = HIGH;
const int CLOCKWISE = LOW;
const int stepperMicrosDelay = 50; //Time delay between pulses in microseconds
const int numPulses = 680;

//reachAndGrab/reachAndDrop function Constants
const float initialAdjMidTarget = 150;
const float initialAdjBaseTarget = 115;
const float midAdjMidTarget = 130;
const float midAdjBaseTarget = 105;
const float finalAdjMidTarget = 100;
const float finalAdjBaseTarget = 95;

const float midGrabTarget = 10;
const float baseGrabTarget = 135;

//Holding a passenger?
static bool holding = false;

void ArmAndStepper::init(){
    pinMode(DIR_PIN,OUTPUT);
    pinMode(PULSE_PIN,OUTPUT);
    baseTarget = BASE_REST_POSITION;
    midTarget = MID_REST_POSITION;
    lastPropErr = 0.;
}

void ArmAndStepper::doControl(){
    angle = ArmAndStepper::getAngle();
    
    now = micros();
    propErr = angle - baseTarget;
    derivErr = (propErr - lastPropErr)/(now - lastTime) * 1000000.;
    intErr += (propErr + lastPropErr)/2 * (now - lastTime) / 1000000.;
    ArmAndStepper::setBaseMotor((long) ArmAndStepper::getControlValue());
    
    RCServo0.write(midTarget);
    
    lastTime = now;
    lastPropErr = propErr;
}

float ArmAndStepper::getAngle() {
    float voltage = (float) analogRead(POTENTIOMETER) * 5./1024.;
    return 130.814 * (3. * voltage - 10.) / (voltage - 5.) + 60.;
}

void ArmAndStepper::setBaseMotor(long duty){
    if(duty > 255){
        duty = 255;
    }
    else if(duty < -255){
        duty = -255;
    }
    motor.speed(MOTOR_PIN_ARM,duty);
}

float ArmAndStepper::getControlValue(){
    float control = 0;
    if(holding){
        control += HOLD_PROP_GAIN * propErr;
    }else{
        control += PROP_GAIN * propErr;
    }
    control += DERIV_GAIN * derivErr;
    control += INT_GAIN * intErr;
    return control;
}


void ArmAndStepper::grab(){
    
    //If switches are already triggered, then do nothing
    if(!digitalRead(ARM_SWITCHES[0]) || !digitalRead(ARM_SWITCHES[1])){}
    
    else{
        motor.speed(MOTOR_PIN_BABY,190);
        unsigned long startTime = millis();
        while(true){
            ArmAndStepper::doControl();
            if(!digitalRead(ARM_SWITCHES[0])){
                holding = true;
                break;
            }
            if(!digitalRead(ARM_SWITCHES[1])){
                holding = false;
                break;
            }
            if(millis() - startTime > 2000){
                holding = true;
                break;
            }
        }
        if(holding){
            motor.speed(MOTOR_PIN_BABY,140);
        }else{
            ArmAndStepper::drop();
        }
    }
}


void ArmAndStepper::drop(){
    motor.speed(MOTOR_PIN_BABY,-140);
    holding = false;
    delay(dropTime);
    motor.speed(MOTOR_PIN_BABY,0);
}

void ArmAndStepper::reachAndClaw(bool grabbing)
{
    midTarget = initialAdjMidTarget;
    baseTarget = initialAdjBaseTarget;
    unsigned long startTime = millis();
    while (millis() - startTime < 500){ 
        ArmAndStepper::doControl();
        LCD.clear();
        LCD.print(ArmAndStepper::getAngle());
    }
    
    midTarget = midAdjMidTarget;
    baseTarget = midAdjBaseTarget;
    
    while (millis() - startTime < 1000){ 
        ArmAndStepper::doControl();
        LCD.clear();
        LCD.print(ArmAndStepper::getAngle());
    }
    baseTarget = finalAdjBaseTarget; 
    midTarget = finalAdjMidTarget;
    while (millis() - startTime < 1500) {  
        ArmAndStepper::doControl();
        LCD.clear();
        LCD.print(ArmAndStepper::getAngle());
        if (!digitalRead(ARM_SWITCHES[2])) {
            baseTarget = ArmAndStepper::getAngle() + 5;
            break;
        } 
    }
    if (grabbing) 
        ArmAndStepper::grab();
    else 
        ArmAndStepper::drop();
    ArmAndStepper::setRestPosition();
}

void ArmAndStepper::refinedReachAndGrab(){
  midTarget = midGrabTarget;
  baseTarget = baseGrabTarget;
  unsigned long startTime = millis();
    while(true){
      ArmAndStepper::doControl();
      if(digitalRead(ARM_SWITCHES[2]) 
      && ((millis()-startTime)>250)){
        if(baseTarget > 95){
          baseTarget -= 5;
        }
        if(midTarget < 120 && baseTarget <= 115)
          midTarget += 10;
        startTime = millis();
      }
      if(!digitalRead(ARM_SWITCHES[2]) || (baseTarget <= 95 && midTarget >= 120))
        break;
      delay(5);
    }
    ArmAndStepper::grab();
    ArmAndStepper::setRestPosition();
}


void ArmAndStepper::setRestPosition(){
    unsigned long startTime = millis();
    baseTarget = finalAdjBaseTarget;
    midTarget = finalAdjMidTarget;
    while(millis() - startTime < 1000){
      ArmAndStepper::doControl();
    }
    
    baseTarget = midAdjBaseTarget; 
    midTarget = midAdjMidTarget;
    while (millis() - startTime < 2000) 
        ArmAndStepper::doControl();
    
    baseTarget = initialAdjBaseTarget;
    midTarget = initialAdjMidTarget;
    while (millis() - startTime < 3000)  // TODO: no hard coding
        ArmAndStepper::doControl();
    
    if (holding) {
        baseTarget = BASE_HOLD_POSITION;
        midTarget = MID_HOLD_POSITION;
    } else {
        baseTarget = BASE_REST_POSITION;
        midTarget = MID_REST_POSITION;
    }
}

//Turns the stepper motor a specified number of steps
void ArmAndStepper::stepperTurn(bool CW,int count){
    if(CW){
        digitalWrite(DIR_PIN,CLOCKWISE);
    } else{
        digitalWrite(DIR_PIN,COUNTERCLOCKWISE);
    }
    int i;
    unsigned long prevTime = millis();
    for(i = 0; i < count; i++){
        if(millis() - prevTime > 10){
            ArmAndStepper::doControl();
            prevTime = millis();
        }
        digitalWrite(PULSE_PIN,HIGH);
        delayMicroseconds(stepperMicrosDelay);
        
        digitalWrite(PULSE_PIN,LOW);
        delayMicroseconds(stepperMicrosDelay);

        LCD.clear();
        LCD.print(ArmAndStepper::getAngle());
    }
}

/* 
 * Parameter: turnRight - Turn right if true, left otherwise
 * Parameter: grab - grab if true, drop otherwise
 */
void ArmAndStepper::turnAndReach(bool turnRight, bool grab){
    LCD.clear();
    LCD.print("initial turn");
    ArmAndStepper::stepperTurn(turnRight, numPulses);
    LCD.clear();
    LCD.print("grabbing in turn and reach");
    if(grab)
      ArmAndStepper::refinedReachAndGrab();
    else
      ArmAndStepper::reachAndClaw(grab);
    LCD.clear();
    LCD.print("turning back");
    ArmAndStepper::stepperTurn(!turnRight, numPulses);
}

bool ArmAndStepper::isHolding(){
  return holding;
}

void ArmAndStepper::setHolding(bool newHolding){
  holding = newHolding;
}

