#include <phys253.h>
#include <StandardCplusplus.h>
#include <string>
#include "pins.hpp"
#include "ArmControl.hpp"

//Analog pin for base angle potentiometer signal
const int BASE_ANGLE_PIN = 0;
//Base GBBC motor output number
const int BASE_MOTOR_NUMBER = 0;

//Claw's Baby Motor output number
const int BABY_MOTOR_NUM = 1;
//Digital pin indicating a successfully caught animal
const int CATCH_SWITCH = 0;
//Digital pin indicating a claw closing on itself
const int NO_CATCH_SWITCH = 1;
//Digital pin to detect when an animal is in the claw
const int INNER_CLAW_SWITCH = 2;
//Amount of time to run motor to drop animal
const int DROP_TIME = 400; // Milliseconds

//SPECIFIED GAIN VALUES
const float PROP_GAIN = 10.;
const float HOLD_PROP_GAIN = 17.;
const float INT_GAIN = 0.;
const float DERIV_GAIN = 0.5;

//Dynamic Control Variables
float baseTarget, midTarget;
float propErr, derivErr, intErr, lastPropErr;
float angle;
float currAngle = 0;
float now, lastTime;
int hasInitialized = 0;

//Rest Positions
const float BASE_REST_POSITION = 75;
const float MID_REST_POSITION = 120;
const float BASE_HOLD_POSITION = 85;
const float MID_HOLD_POSITION = 170;

//Iterator for LCD printing
int LCDControl;

//Stepper Constants
const int STEPPER_DIR_PIN = 8;
const int STEPPER_PULSE_PIN = 9;
const int COUNTERCLOCKWISE = HIGH;
const int CLOCKWISE = LOW;
const int STEPPER_MICROS_DELAY = 40000; //Time delay between pulses in microseconds
const int NUM_PULSES = 700;

//reachAndClaw/reachAndDrop function Constants
const float INITIAL_ADJ_MID_TARGET = 180;
const float INITIAL_ADJ_BASE_TARGET = 40;
const float FINAL_ADJ_MID_TARGET = 180;
const float FINAL_ADJ_BASE_TARGET = 25;

//Holding a passenger?
bool holding = false;

void ArmControl::init() {

  
  #include <phys253setup.txt>
  //portMode(1,OUTPUT);
  pinMode(stepperDirPin,OUTPUT);
  pinMode(stepperPulsePin,OUTPUT);
  Serial.begin(9600);
  this->baseTarget = this->baseRestPosition;
  this->midTarget = this->midRestPosition;
  this->lastPropErr = 0.;
  this->LCDControl = 1;
}

ArmControl::ArmControl()
	: MinorMode(),
	  
	  dropTime(DROP_TIME),
	  propGain(PROP_GAIN),
	  holdPropGain(HOLD_PROP_GAIN),
	  intGain(INT_GAIN),
	  derivGain(DERIV_GAIN),
	  baseRestPosition(BASE_REST_POSITION),
	  midRestPosition(MID_REST_POSITION),
	  baseHoldPosition(BASE_HOLD_POSITION),
	  midHoldPosition(MID_HOLD_POSITION),
	  stepperMicrosDelay(STEPPER_MICROS_DELAY),
	  numPulses(NUM_PULSES),
	  initialAdjMidTarget(INITIAL_ADJ_MID_TARGET),
	  initialAdjBaseTarget(INITIAL_ADJ_BASE_TARGET),
	  finalAdjMidTarget(FINAL_ADJ_MID_TARGET),
	  finalAdjBaseTarget(FINAL_ADJ_BASE_TARGET),
	  
	  //pin constants
	  baseAnglePin(pins::POTENTIOMETER),
	  baseMotorNumber(pins::MOTOR_PIN_ARM),
	  babyMotorNum(pins::MOTOR_PIN_BABY),
	  catchSwitch(pins::ARM_SWITCHES[0]),
	  noCatchSwitch(pins::ARM_SWITCHES[1]),
	  innerClawSwitch(pins::ARM_SWITCHES[2]),
	  stepperDirPin(pins::DIR_PIN),
	  stepperPulsePin(pins::PULSE_PIN)
	  {
      this->init();
	  }
	  
	  
	  

void ArmControl::loop() {
  doControl();

  delay(10);
}

// The control loop. Should be implemented wherever the code
//be for extended perids of time to prevent arm from overcorrecting,
//grinding gears, etc.
// Should be repeated with several millisecond delays for integral/
//derivative control purposes
void ArmControl::doControl(){
  angle = getAngle();

  now = micros();
  propErr = angle - baseTarget;
  derivErr = (propErr - lastPropErr)/(now - lastTime) * 1000000.;
  intErr += (propErr + lastPropErr)/2 * (now - lastTime) / 1000000.;
  setBaseMotor((int) getControlValue());

  RCServo0.write(midTarget);
  
  lastTime = now;
  lastPropErr = propErr;

  /*
  if(LCDControl % 25 == 0){
    //printState();
    LCDControl = 1;
  }
  LCDControl++;
  */
}

//Converts base potentiometer voltage to corresponding angle
float ArmControl::getAngle() {
  float voltage = (float) analogRead(BASE_ANGLE_PIN) * 5./1024.;
  return 130.814*(3.*voltage - 10.)/(voltage-5.)+27.5;
}

//Wrapper function for setting motor speed
//Prevents values larger than 255 in either direction
void ArmControl::setBaseMotor(int duty){
  if(duty > 255){
    duty = 255;
  }
  else if(duty < -255){
    duty = -255;
  }
  motor.speed(BASE_MOTOR_NUMBER,duty);
}

//Returns the motor speed based on PID control
float ArmControl::getControlValue(){
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

//Closes the claw until something is detected in claw, the claw
//closes on itself or a timeout is reached
void ArmControl::grabShit(){

  //If switches are already triggered, then do nothing
  if(!digitalRead(CATCH_SWITCH) || !digitalRead(NO_CATCH_SWITCH)){}
  
  else{
    LCD.clear();
    LCD.print("Grabbing");
    motor.speed(BABY_MOTOR_NUM,140);
    unsigned long startTime = millis();
    while(1){
      doControl();
      if(!digitalRead(CATCH_SWITCH)){
        holding = true;
        break;
      }
      if(!digitalRead(NO_CATCH_SWITCH)){
        holding = false;
        break;
      }
      if(millis() - startTime > 2000){
        holding = true;
        break;
      }
    }
    if(holding){
      motor.speed(BABY_MOTOR_NUM,20);
    }else{
      dropShit();
    }
  }
}

//Opens the claw for specified time
void ArmControl::dropShit(){
  motor.speed(BABY_MOTOR_NUM,-140);
  holding = false;
  delay(DROP_TIME);
  motor.speed(BABY_MOTOR_NUM,0);
}

/*//Function to update the LCD periodically
void printState(){
  LCD.clear();
  LCD.print("A");
  LCD.print(getAngle());
  LCD.print(" TaR:");
  if(knob(7) > 900){
    LCD.print("R");
  } else if (knob(7) < 150){
    LCD.print("L");
  } else {
    LCD.print("S");
  }
}*/

//Extends arm over two periods and either grabs or drops
void ArmControl::reachAndClaw(bool grabbing){
  
  //baseTarget = INITIAL_ADJ_BASE_TARGET; 
  midTarget = INITIAL_ADJ_MID_TARGET;
  unsigned long startTime = millis();
  while(millis() - startTime < 500){
    doControl();
  }

  baseTarget = INITIAL_ADJ_BASE_TARGET;

  while(millis() - startTime < 1000){
    doControl();
  }
  
  baseTarget = FINAL_ADJ_BASE_TARGET; 
  midTarget = FINAL_ADJ_MID_TARGET;
  startTime = millis();
  while(millis() - startTime < 2500){
    doControl();
    if(!digitalRead(INNER_CLAW_SWITCH)){
      baseTarget = getAngle() + 5;
      break;
    }
  }
  if (grabbing) {
	  grabShit();
  }
  else {
	  dropShit();
  }
  setRestPosition();
}

//Sets the control target values to rest position
void ArmControl::setRestPosition(){
  if(holding){
    baseTarget = BASE_HOLD_POSITION;
    midTarget = MID_HOLD_POSITION;
  }else{
    baseTarget = BASE_REST_POSITION;
    midTarget = MID_REST_POSITION;
  }
  
}

//Turns the stepper motor a specified number of steps
void ArmControl::stepperTurn(bool CW,int count){
  if(CW){
    digitalWrite(STEPPER_DIR_PIN,CLOCKWISE);
  } else{
    digitalWrite(STEPPER_DIR_PIN,COUNTERCLOCKWISE);
  }
  int i;
  unsigned long prevTime = millis();
  for(i = 0; i < count; i++){
    if(millis() - prevTime > 10){
      doControl();
      prevTime = millis();
    }
    digitalWrite(STEPPER_PULSE_PIN,HIGH);
    delayMicroseconds(STEPPER_MICROS_DELAY);
    
    digitalWrite(STEPPER_PULSE_PIN,LOW);
    delayMicroseconds(STEPPER_MICROS_DELAY);
  }
}

/* 
 * Parameter: turnRight - Turn right if true, left otherwise
 * Parameter: grab - grab if true, drop otherwise
 */
void ArmControl::turnAndReach(bool turnRight, bool grab){
  stepperTurn(turnRight, NUM_PULSES);
  reachAndClaw(grab);
  stepperTurn(!turnRight, NUM_PULSES);
}

bool isHolding(){
  return holding;
}

