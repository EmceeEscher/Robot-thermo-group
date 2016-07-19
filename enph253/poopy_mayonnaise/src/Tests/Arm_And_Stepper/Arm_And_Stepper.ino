#include <phys253.h>
#include <phys253pins.h>

//Analog pin to detect measure potentiometer
const int baseAnglePin = 0;
//Base GBBC motor output number
const int baseMotorNumber = 0;

//Claw's Baby Motor output number
const int babyMotorNum = 1;
//Digital pin indicating a successfully caugh animal
const int catchSwitch = 0;
//Digital pin indicating a claw closing on itself
const int noCatchSwitch = 1;
//Amount of time to run motor to drop animal
const int dropTime = 400; // Milliseconds

const float propGain = 6.;
const float intGain = 0.;
const float derivGain = 0.;

float baseTarget, midTarget;
float propErr, derivErr, intErr, lastPropErr;
float angle;
float now, lastTime;

//Rest Positions
const float baseRestPosition = 80;
const float midRestPosition = 50;

//Iterator for LCD output
int i;

//Stepper Variables
const int stepperDirPin = 8;
const int stepperPulsePin = 9;
const int COUNTERCLOCKWISE = HIGH;
const int CLOCKWISE = LOW;

void setup() {
  #include <phys253setup.txt>
  portMode(1,OUTPUT);
  pinMode(stepperDirPin,OUTPUT);
  pinMode(stepperPulsePin,OUTPUT);
  Serial.begin(9600);
  baseTarget = 80.;
  midTarget = 90.;
  lastPropErr = 0.;
  i = 1;
}

void loop() {
  doControl();

  if(startbutton()){
    fullGrabMotion();
  } else if (stopbutton()){
    reachAndDrop();
  } else if(knob(6) > 950){
    stepperTurn(true,10);
  } else if(knob(6) < 100){
    stepperTurn(false,10);
  }

  if(Serial.available() > 0){
    //TODO, add any serial input processing here
  }
  delay(10);
}

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

  if(i % 25 == 0){
    printState();
    i = 1;
  }
  i++;
}

float getAngle() {
  float voltage = (float) analogRead(baseAnglePin) * 5./1024.;
  return ((-142.857 * voltage) / (voltage - 5.));
  //return ((-142.857 * voltage) / (voltage - 5.));
}

//Wrapper function for setting motor speed
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
  control += propGain * propErr;
  control += derivGain * derivErr;
  control += intGain * intErr;
  return control;
}

void grabShit(){
  
  if(!digitalRead(catchSwitch) || !digitalRead(noCatchSwitch)){
    
  } else{
    LCD.clear();
    LCD.print("Grabbing");
    motor.speed(babyMotorNum,140);
    unsigned long startTime = millis();
    while(1){
      if(!digitalRead(catchSwitch)){
        break;
      }
      if(!digitalRead(noCatchSwitch)){
        break;
      }
      if(millis() - startTime > 2000){
        break;
      }
    }
    motor.speed(babyMotorNum,0);
  }
}

void dropShit(){
  motor.speed(babyMotorNum,-140);
  delay(dropTime);
  motor.speed(babyMotorNum,0);
}


void printState(){
  LCD.clear();
  LCD.print("A");
  LCD.print(getAngle());
}

void fullGrabMotion(){
  
  baseTarget = 40; midTarget = 45;
  unsigned long startTime = millis();
  while(millis() - startTime < 500){
    doControl();
  }

  baseTarget = 5; midTarget = 0;
  startTime = millis();
  while(millis() - startTime < 3000){
    doControl();
  }
  grabShit();
  setRestPosition();
}

void reachAndDrop(){
  
  baseTarget = 40; midTarget = 45;
  unsigned long startTime = millis();
  while(millis() - startTime < 500){
    doControl();
  }

  baseTarget = 5; midTarget = 0;
  startTime = millis();
  while(millis() - startTime < 3000){
    doControl();
  }
  dropShit();
  setRestPosition();
}

void setRestPosition(){
  baseTarget = baseRestPosition;
  midTarget = midRestPosition;
}

void stepperTurn(bool CW,int count){
  if(CW){
    digitalWrite(stepperDirPin,CLOCKWISE);
  } else{
    digitalWrite(stepperDirPin,COUNTERCLOCKWISE);
  }
  for(i = 0; i < count; i++){
    digitalWrite(stepperPulsePin,HIGH);
    doControl();
    delay(5);
    digitalWrite(stepperPulsePin,LOW);
    doControl();
    delay(5);
  }
}
