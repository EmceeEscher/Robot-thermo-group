#include <phys253.h>
#include <phys253pins.h>

//Analog pin for base angle potentiometer signal
const int baseAnglePin = 0;
//Base GBBC motor output number
const int baseMotorNumber = 0;

//Claw's Baby Motor output number
const int babyMotorNum = 1;
//Digital pin indicating a successfully caugh animal
const int catchSwitch = 0;
//Digital pin indicating a claw closing on itself
const int noCatchSwitch = 1;
//Digital pin to detect when an animal is in the claw
const int innerClawSwitch = 2;
//Amount of time to run motor to drop animal
const int dropTime = 400; // Milliseconds

//SPECIFIED GAIN VALUES
const float propGain = 10.;
const float holdPropGain = 17.;
const float intGain = 0.;
const float derivGain = 0.5;

//Dynamic Control Variables
float baseTarget, midTarget;
float propErr, derivErr, intErr, lastPropErr;
float angle;
float now, lastTime;

//Rest Positions
const float baseRestPosition = 75;
const float midRestPosition = 120;
const float baseHoldPosition = 85;
const float midHoldPosition = 170;

//Iterator for LCD printing
int LCDControl;

//Stepper Constants
const int stepperDirPin = 8;
const int stepperPulsePin = 9;
const int COUNTERCLOCKWISE = HIGH;
const int CLOCKWISE = LOW;
const int stepperMicrosDelay = 40000; //Time delay between pulses in microseconds
const int numPulses = 700;

//reachAndGrab/reachAndDrop function Constants
const float initialAdjMidTarget = 180;
const float initialAdjBaseTarget = 40;
const float finalAdjMidTarget = 180;
const float finalAdjBaseTarget = 25;

//Holding a passenger?
bool holding = false;

void setup() {
  #include <phys253setup.txt>
  portMode(1,OUTPUT);
  pinMode(stepperDirPin,OUTPUT);
  pinMode(stepperPulsePin,OUTPUT);
  Serial.begin(9600);
  baseTarget = baseRestPosition;
  midTarget = midRestPosition;
  lastPropErr = 0.;
  LCDControl = 1;
}

void loop() {
  doControl();

  if(startbutton()){
    if(knob(7) > 900){
      turnAndReach(true,true);
    } else if(knob(7) < 150){
      turnAndReach(false,true);
    } else{
      reachAndGrab();
    }
  } else if (stopbutton()){
    if(knob(7) > 900){
      turnAndReach(true,false);
    } else if(knob(7) < 150){
      turnAndReach(false,false);
    } else{
      reachAndDrop();
    }
  } else if(knob(6) > 900){
    stepperTurn(true, 20);
  } else if(knob(6) < 150){
    stepperTurn(false, 20);
  }

  if(Serial.available() > 0){
    //TODO, add any serial input processing here
  }
  delay(10);
}

// The control loop. Should be implemented wherever the code
//be for extended perids of time to prevent arm from overcorrecting,
//grinding gears, etc.
// Should be repeated with several millisecond delays for integral/
//derivative control purposes
void doControl(){
  angle = getAngle();

  now = micros();
  propErr = angle - baseTarget;
  derivErr = (propErr - lastPropErr)/(now - lastTime) * 1000000.;
  intErr += (propErr + lastPropErr)/2 * (now - lastTime) / 1000000.;
  setBaseMotor((int) getControlValue());

  RCServo0.write(midTarget);
  
  lastTime = now;
  lastPropErr = propErr;

  
  if(LCDControl % 25 == 0){
    printState();
    LCDControl = 1;
  }
  LCDControl++;
}

//Converts base potentiometer voltage to corresponding angle
float getAngle() {
  float voltage = (float) analogRead(baseAnglePin) * 5./1024.;
  return 130.814*(3.*voltage - 10.)/(voltage-5.)+27.5;
  //return ((-142.857 * voltage) / (voltage - 5.));
}

//Wrapper function for setting motor speed
//Prevents values larger than 255 in either direction
void setBaseMotor(int duty){
  if(duty > 255){
    duty = 255;
  }
  else if(duty < -255){
    duty = -255;
  }
  motor.speed(baseMotorNumber,duty);
}

//Returns the motor speed based on PID control
float getControlValue(){
  float control = 0;
  if(holding){
    control += holdPropGain * propErr;
  }else{
    control += propGain * propErr;
  }
  control += derivGain * derivErr;
  control += intGain * intErr;
  return control;
}

//Closes the claw until something is detected in claw, the claw
//closes on itself or a timeout is reached
void grabShit(){

  //If switches are already triggered, then do nothing
  if(!digitalRead(catchSwitch) || !digitalRead(noCatchSwitch)){}
  
  else{
    LCD.clear();
    LCD.print("Grabbing");
    motor.speed(babyMotorNum,140);
    unsigned long startTime = millis();
    while(1){
      doControl();
      if(!digitalRead(catchSwitch)){
        holding = true;
        break;
      }
      if(!digitalRead(noCatchSwitch)){
        holding = false;
        break;
      }
      if(millis() - startTime > 2000){
        holding = true;
        break;
      }
    }
    motor.speed(babyMotorNum,0);
  }
}

//Opens the claw for specified time
void dropShit(){
  motor.speed(babyMotorNum,-140);
  holding = false;
  delay(dropTime);
  motor.speed(babyMotorNum,0);
}

//Function to update the LCD periodically
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
}

//Extends arm over two periods and calls grab function
void reachAndGrab(){
  
  //baseTarget = initialAdjBaseTarget; 
  midTarget = initialAdjMidTarget;
  unsigned long startTime = millis();
  while(millis() - startTime < 500){
    doControl();
  }

  baseTarget = initialAdjBaseTarget;

  while(millis() - startTime < 1000){
    doControl();
  }
  
  baseTarget = finalAdjBaseTarget; 
  midTarget = finalAdjMidTarget;
  startTime = millis();
  while(millis() - startTime < 2500){
    doControl();
    if(!digitalRead(innerClawSwitch)){
      baseTarget = getAngle() + 5;
      break;
    }
  }
  grabShit();
  setRestPosition();
}

//equivalent to reachAndGrab, but calls drop function
void reachAndDrop(){
  
  //baseTarget = initialAdjBaseTarget; 
  midTarget = initialAdjMidTarget;
  unsigned long startTime = millis();
  while(millis() - startTime < 500){
    doControl();
  }

  baseTarget = initialAdjBaseTarget;
  while(millis() - startTime < 1000){
    doControl();
  }
  
  baseTarget = finalAdjBaseTarget; 
  midTarget = finalAdjMidTarget;
  startTime = millis();
  while(millis() - startTime < 2500){
    doControl();
  }
  dropShit();
  setRestPosition();
}

//Sets the control target values to rest position
void setRestPosition(){
  if(holding){
    baseTarget = baseHoldPosition;
    midTarget = midHoldPosition;
  }else{
    baseTarget = baseRestPosition;
    midTarget = midRestPosition;
  }
  
}

//Turns the stepper motor a specified number of steps
void stepperTurn(bool CW,int count){
  if(CW){
    digitalWrite(stepperDirPin,CLOCKWISE);
  } else{
    digitalWrite(stepperDirPin,COUNTERCLOCKWISE);
  }
  int i;
  unsigned long prevTime = millis();
  for(i = 0; i < count; i++){
    if(millis() - prevTime > 10){
      doControl();
      prevTime = millis();
    }
    digitalWrite(stepperPulsePin,HIGH);
    delayMicroseconds(stepperMicrosDelay);
    
    digitalWrite(stepperPulsePin,LOW);
    delayMicroseconds(stepperMicrosDelay);
  }
}

/* 
 * Parameter: turnRight - Turn right if true, left otherwise
 * Parameter: grab - grab if true, drop otherwise
 */
void turnAndReach(bool turnRight, bool grab){
  stepperTurn(turnRight, numPulses);
  grab ? reachAndGrab() : reachAndDrop();
  stepperTurn(!turnRight, numPulses);
}

