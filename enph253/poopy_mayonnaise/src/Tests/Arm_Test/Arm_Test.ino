#include <phys253.h>

/*
 * Notes:
 * Gains seem reasonable around 0 < gain < 1.1
 * 
 */

const int baseAnglePin = 0;

float propGain, derivGain, intGain;
float baseTarget, midTarget;
float propErr, derivErr, intErr, lastPropErr;
float angle;
float now, lastTime;
Tinah tinah;

int i;

void setup() {
  Serial.begin(9600);
  propGain = 0.; 
  derivGain = 0.; 
  intGain = 0.;
  baseTarget = 45.;
  midTarget = 0;
  lastPropErr = 0;
  tinah.RCServo0.write(midTarget);
  i = 0;
}

void loop() {
  angle = getAngle();
  now = micros();
  
  propErr = angle - baseTarget;
  derivErr = (propErr - lastPropErr)/(now - lastTime) * 1000000.;
  intErr += (propErr + lastPropErr)/2 * (now - lastTime) / 1000000.;
  setBaseMotor((int) getControl());
  
  lastTime = now;
  lastPropErr = propErr;

  if(i % 250 == 0){
    tinah.LCD.clear();
    tinah.LCD.print(getControl());
    tinah.LCD.setCursor(0,1);
    tinah.LCD.print((int) getAngle());
    i = 1;
  }
  
  if(stopbutton()){
    serialMenu();
    lastTime = micros();
    intErr = 0;
    tinah.RCServo0.write(midTarget);
  }

  i++;
}

//TODO: Actually determine a formula to get the angle
float getAngle() {
  float voltage = (float) analogRead(baseAnglePin) * 5./1024.;
  return ((-142.857 * voltage) / (voltage - 5.));
}

//Wrapper function for setting motor speed
void setBaseMotor(int duty){
  if(duty > 255){
    duty = 255;
  }
  else if(duty < -255){
    duty = -255;
  }
  tinah.motor.speed(0,duty);
}

//Returns the motor speed based on PID control
float getControl(){
  float control = 0;
  control += propGain * propErr;
  control += derivGain * derivErr;
  control += intGain * intErr;
  return control;
}

//Settings Menu
void settingsMenu(){
  float lastPropGain = propGain;
  float lastDerivGain = derivGain;
  float lastIntGain = intGain;
  
  float origPropGain = propGain;
  float origDerivGain = derivGain;
  float origIntGain = intGain;
  
  tinah.LCD.clear();
  tinah.LCD.print("Settings");
  while(startbutton()){}// Wait until button is released

  tinah.LCD.clear();
  tinah.LCD.print("P: ");
  tinah.LCD.print(propGain);
  tinah.LCD.setCursor(0,1);
  tinah.LCD.print("Orig: ");
  tinah.LCD.print(origPropGain);
  while(1){
    propGain = (float) knob(6) / 200; //Determine new propGain
    if(propGain != lastPropGain){
      tinah.LCD.clear();
      tinah.LCD.print("P: ");
      tinah.LCD.print(propGain);
      tinah.LCD.setCursor(0,1);
      tinah.LCD.print("Orig: ");
      tinah.LCD.print(origPropGain);

      lastPropGain = propGain;
    }
    if(startbutton()){
      break;
    }
  }
  tinah.LCD.clear();
  while(startbutton()){}

  tinah.LCD.print("I: ");
  tinah.LCD.print(intGain);
  tinah.LCD.setCursor(0,1);
  tinah.LCD.print("Orig: ");
  tinah.LCD.print(origIntGain);
  while(1){
    intGain = (float) knob(6) / 200; //Determine new intGain
    if(intGain != lastIntGain){
      tinah.LCD.clear();
      tinah.LCD.print("I: ");
      tinah.LCD.print(intGain);
      tinah.LCD.setCursor(0,1);
      tinah.LCD.print("Orig: ");
      tinah.LCD.print(origIntGain);

      lastIntGain = intGain;
    }
    if(startbutton()){
      break;
    }
  }
  tinah.LCD.clear();
  while(startbutton()){}

  tinah.LCD.print("D: ");
  tinah.LCD.print(derivGain);
  tinah.LCD.setCursor(0,1);
  tinah.LCD.print("Orig: ");
  tinah.LCD.print(origDerivGain);
  while(1){
    derivGain = (float) knob(6) / 200; //Determine new derivGain
    if(derivGain != lastDerivGain){
      tinah.LCD.clear();
      tinah.LCD.print("D: ");
      tinah.LCD.print(derivGain);
      tinah.LCD.setCursor(0,1);
      tinah.LCD.print("Orig: ");
      tinah.LCD.print(origDerivGain);

      lastDerivGain = derivGain;
    }
    if(startbutton()){
      break;
    }
  }
  tinah.LCD.clear();
  while(startbutton()){}

}

void serialMenu(){

  tinah.LCD.clear();
  tinah.LCD.print("Serial Menu");
  delay(800);

  //Proportional Gain
  tinah.LCD.clear();
  tinah.LCD.print("P:");
  tinah.LCD.setCursor(0,1);
  tinah.LCD.print("Orig: ");
  tinah.LCD.print(propGain);
  while(1){
    if(Serial.available() > 0){
      propGain = Serial.parseFloat();
      break;
    }
    else{
      delay(1);
    }
  }
  tinah.LCD.clear();
  tinah.LCD.print("P: ");
  tinah.LCD.print(propGain);
  delay(800);

  //Integral Gain
  tinah.LCD.clear();
  tinah.LCD.print("I:");
  tinah.LCD.setCursor(0,1);
  tinah.LCD.print("Orig: ");
  tinah.LCD.print(intGain);
  while(1){
    if(Serial.available() > 0){
      intGain = Serial.parseFloat();
      break;
    }
    else{
      delay(1);
    }
  }
  tinah.LCD.clear();
  tinah.LCD.print("I: ");
  tinah.LCD.print(intGain);
  delay(800);

  //Derivative Gain
  tinah.LCD.clear();
  tinah.LCD.print("D:");
  tinah.LCD.setCursor(0,1);
  tinah.LCD.print("Orig: ");
  tinah.LCD.print(derivGain);
  while(1){
    if(Serial.available() > 0){
      derivGain = Serial.parseFloat();
      break;
    }
    else{
      delay(1);
    }
  }
  tinah.LCD.clear();
  tinah.LCD.print("D: ");
  tinah.LCD.print(derivGain);
  delay(800);

  //Base Target
  tinah.LCD.clear();
  tinah.LCD.print("Base:");
  tinah.LCD.setCursor(0,1);
  tinah.LCD.print("Orig: ");
  tinah.LCD.print(baseTarget);
  while(1){
    if(Serial.available() > 0){
      baseTarget = Serial.parseFloat();
      break;
    }
    else{
      delay(1);
    }
  }
  tinah.LCD.clear();
  tinah.LCD.print("Base: ");
  tinah.LCD.print(baseTarget);
  delay(800);

  //Middle Joint Target
  tinah.LCD.clear();
  tinah.LCD.print("Mid:");
  tinah.LCD.setCursor(0,1);
  tinah.LCD.print("Orig: ");
  tinah.LCD.print(midTarget);
  while(1){
    if(Serial.available() > 0){
      midTarget = Serial.parseFloat();
      break;
    }
    else{
      delay(1);
    }
  }
  tinah.LCD.clear();
  tinah.LCD.print("Mid: ");
  tinah.LCD.print(midTarget);
  delay(800);
  
}
