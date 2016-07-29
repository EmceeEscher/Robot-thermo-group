#include <phys253.h>
#include <StandardCplusplus.h>
#include <string>
#include "pins.hpp"
#include "ArmControl.hpp"

//Amount of time to run motor to drop animal
const int DROP_TIME = 400; // Milliseconds

//SPECIFIED GAIN VALUES
const float PROP_GAIN = 10.;
const float HOLD_PROP_GAIN = 17.;
const float INT_GAIN = 0.;
const float DERIV_GAIN = 1.5;

//Dynamic Control Variables  // TODO: get rid of these
float baseTarget, midTarget;
float propErr, derivErr, intErr, lastPropErr;
float angle;
float now, lastTime;

//Rest Positions
const float BASE_REST_POSITION = 70;
const float MID_REST_POSITION = 170;
const float BASE_HOLD_POSITION = 70;
const float MID_HOLD_POSITION = 170;

//Iterator for LCD printing, not currently used  // TODO: get rid of this
int LCDControl;

//Stepper Constants
const int COUNTERCLOCKWISE = HIGH;
const int CLOCKWISE = LOW;
const int STEPPER_MICROS_DELAY = 1600; //Time delay between pulses in microseconds
const int NUM_PULSES = 680;

//reachAndClaw/reachAndDrop function Constants
const float INITIAL_ADJ_MID_TARGET = 100;
const float INITIAL_ADJ_BASE_TARGET = 60;
const float MID_ADJ_MID_TARGET = 135;
const float MID_ADJ_BASE_TARGET = 40;
const float FINAL_ADJ_MID_TARGET = 170;
const float FINAL_ADJ_BASE_TARGET = 20;

//Holding a passenger?
//bool holding = false;


void ArmControl::init()
{
    RCServo1.detach();
    pinMode(this->stepperPulsePin,OUTPUT);
    RCServo2.detach();
    pinMode(this->stepperDirPin,OUTPUT);
    
    Serial.begin(9600);
    baseTarget = this->baseRestPosition;
    midTarget = this->midRestPosition;
    lastPropErr = 0.;
    LCDControl = 1;
    this->holding = false;
}


ArmControl::ArmControl()
    : MinorMode(),
	  
      dropTime             (DROP_TIME),
      propGain             (PROP_GAIN),
      holdPropGain         (HOLD_PROP_GAIN),
      intGain              (INT_GAIN),
      derivGain            (DERIV_GAIN),
      baseRestPosition     (BASE_REST_POSITION),
      midRestPosition      (MID_REST_POSITION),
      baseHoldPosition     (BASE_HOLD_POSITION),
      midHoldPosition      (MID_HOLD_POSITION),
      stepperMicrosDelay   (STEPPER_MICROS_DELAY),
      numPulses            (NUM_PULSES),
      initialAdjMidTarget  (INITIAL_ADJ_MID_TARGET),
      initialAdjBaseTarget (INITIAL_ADJ_BASE_TARGET),
      finalAdjMidTarget    (FINAL_ADJ_MID_TARGET),
      finalAdjBaseTarget   (FINAL_ADJ_BASE_TARGET),
      midAdjMidTarget      (MID_ADJ_MID_TARGET),
      midAdjBaseTarget     (MID_ADJ_BASE_TARGET),
      
      //pin constants
      baseAnglePin     (pins::POTENTIOMETER),
      baseMotorNumber  (pins::MOTOR_PIN_ARM),
      babyMotorNum     (pins::MOTOR_PIN_BABY),
      catchSwitch      (pins::ARM_SWITCHES[0]),
      noCatchSwitch    (pins::ARM_SWITCHES[1]),
      innerClawSwitch  (pins::ARM_SWITCHES[2]),
      stepperDirPin    (pins::DIR_PIN),
      stepperPulsePin  (pins::PULSE_PIN)
{
    this->init();
}


ArmControl::~ArmControl(){}


void ArmControl::loop()
{
    doControl();
    delay(10);  // TODO: no hard coding
}


// The control loop. Should be implemented wherever the code
//be for extended perids of time to prevent arm from overcorrecting,
//grinding gears, etc.
// Should be repeated with several millisecond delays for integral/
//derivative control purposes
void ArmControl::doControl()
{
    angle = getAngle();
    
    now = micros();
    propErr = angle - baseTarget;
    derivErr = (propErr - lastPropErr) / (now - lastTime) * 1000000.;
    intErr += (propErr + lastPropErr) / 2 * (now - lastTime) / 1000000.;
    setBaseMotor(static_cast<int>(getControlValue()));
    
    RCServo0.write(midTarget);
    
    lastTime = now;
    lastPropErr = propErr;
    
    if (LCDControl % 25 == 0) {  // TODO: get rid of hard coding
	this->printState();
	LCDControl = 1;
    }
    ++LCDControl;
}


//Converts base potentiometer voltage to corresponding angle
float ArmControl::getAngle()
{
    // TODO: get rid of hard coding
    float voltage = static_cast<float>(analogRead(this->baseAnglePin) *
				       5./1024.);
    return 130.814 * (3. * voltage - 10.) / (voltage - 5.) + 60.;
}


//Wrapper function for setting motor speed
//Prevents values larger than 255 in either direction
void ArmControl::setBaseMotor(int duty)
{
    if (duty > 255)
	duty = 255;  // TODO: get rid of global variables
    else if(duty < -255)
	duty = -255;
    motor.speed(this->baseMotorNumber,duty);
}


//Returns the motor speed based on PID control
float ArmControl::getControlValue()
{
    float control(0.);
    if (this->holding) 
	control += this->holdPropGain * propErr;  // TODO: get rid of globals
    else
	control += this->propGain * propErr;
    control += this->derivGain * derivErr;
    control += this->intGain * intErr;
    return control;
}


//Closes the claw until something is detected in claw, the claw
//closes on itself or a timeout is reached
void ArmControl::grabShit()
{
    //If switches are already triggered, then do nothing
    if (!digitalRead(this->catchSwitch) || !digitalRead(this->noCatchSwitch)) {
	// TODO: get rid of empty block
    } else {
	LCD.clear();
	LCD.print("Grabbing");
	motor.speed(this->babyMotorNum,140);  // TODO: no hard coding
	unsigned long startTime = millis();
	while (true) {  // TODO: is this a loop inside the main loop?
	    doControl();
	    if (!digitalRead(this->catchSwitch)) {
		this->holding = true;
		break;
	    }
	    if (!digitalRead(this->noCatchSwitch)) {
		this->holding = false;
		break;
	    }
	    if (millis() - startTime > 2000) {  // TODO: no hard coding
		this->holding = true;
		break;
	    }
	}
	if (this->holding)
	    motor.speed(this->babyMotorNum,20);  // TODO: no hard coding
	else
	    dropShit();
    }
}


//Opens the claw for specified time
void ArmControl::dropShit()
{
    motor.speed(this->babyMotorNum,-140);  // TODO: no hard coding
    this->holding = false;
    delay(this->dropTime);  // TODO: is this necessary here?
    motor.speed(this->babyMotorNum,0);
}


//Function to update the LCD periodically
void ArmControl::printState()
{
    LCD.clear();
    LCD.print("A");
    LCD.print(getAngle());
    //LCD.print(RCServo0.attached());
    /*LCD.print(" TaR:");
      if(knob(7) > 900){
      LCD.print("R");
      } else if (knob(7) < 150){
      LCD.print("L");
      } else {
      LCD.print("S");
      }*/
}


//Extends arm over two periods and either grabs or drops
void ArmControl::reachAndClaw(bool grabbing)
{
    midTarget = this->initialAdjMidTarget;
    baseTarget = this->initialAdjBaseTarget;
    unsigned long startTime = millis();
    while (millis() - startTime < 500) // TODO: no hard coding
	doControl();
    // TODO: is it necessary to run all of these loops inside of the main loop
    // or could they be run concurrently
    
    midTarget = this->midAdjMidTarget;
    baseTarget = this->midAdjBaseTarget;
    
    while (millis() - startTime < 1000) // TODO: no hard coding
	doControl();
    
    baseTarget = this->finalAdjBaseTarget; 
    midTarget = this->finalAdjMidTarget;
    while (millis() - startTime < 1500) {  // TODO: no hard coding
	doControl();
	if (!digitalRead(this->innerClawSwitch)) {
	    //baseTarget = getAngle() + 5; // this code don't do nothing
	    break;
	} 
    }
    if (grabbing) 
	grabShit();
    else 
	dropShit();
    setRestPosition();
}


//Sets the control target values to rest position
void ArmControl::setRestPosition(){
    unsigned long startTime = millis();
    baseTarget = this->midAdjBaseTarget; // TODO: no global variables
    midTarget = this->midAdjMidTarget;
    while (millis() - startTime < 500) // TODO: no hard coding
	doControl();
    
    baseTarget = this->initialAdjBaseTarget;
    midTarget = this->initialAdjMidTarget;
    while (millis() - startTime < 1000)  // TODO: no hard coding
	doControl();
    
    if (this->holding) {
	baseTarget = this->baseHoldPosition;
	midTarget = this->midHoldPosition;
    } else {
	baseTarget = this->baseRestPosition;
	midTarget = this->midRestPosition;
    }
}


//Turns the stepper motor a specified number of steps
void ArmControl::stepperTurn(bool CW, int count) // TODO: lowercase params
{
    if (CW) 
	digitalWrite(this->stepperDirPin, CLOCKWISE);
    else
	digitalWrite(this->stepperDirPin, COUNTERCLOCKWISE);
    
    int i;
    unsigned long prevTime = millis();
    for (i = 0; i < count; ++i) {
	if (millis() - prevTime > 10) {
	    doControl();
	    prevTime = millis();
	}
	digitalWrite(this->stepperPulsePin, HIGH);
	delayMicroseconds(this->stepperMicrosDelay);
	
	digitalWrite(this->stepperPulsePin, LOW);
	delayMicroseconds(this->stepperMicrosDelay);
    }
    while (!startbutton())   // TODO: is this going to be in the final version?
	doControl();
}


/* 
 * Parameter: turnRight - Turn right if true, left otherwise
 * Parameter: grab - grab if true, drop otherwise
 */
void ArmControl::turnAndReach(bool turnRight, bool grab)
{
    stepperTurn(turnRight, this->numPulses);  // TODO: no global variables
    reachAndClaw(grab);
    stepperTurn(!turnRight, this->numPulses);
}


bool ArmControl::isHolding()
{
    return this->holding;
}

