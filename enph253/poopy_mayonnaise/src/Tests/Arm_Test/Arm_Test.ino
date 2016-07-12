#include <phys253.h>

/*
 * Notes:
 * Gains seem reasonable around 0 < gain < 1.1
 * 
 */


float propGain, derivGain, intGain;
float baseTarget;
float propErr, derivErr, intErr, lastPropErr;
float angle;
float now, lastTime;
Tinah tinah;

int i;

void setup() {
  propGain = 0.; 
  derivGain = 0.; 
  intGain = 0.;
  baseTarget = 45.;
  lastPropErr = 0;
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

  if(i % 50 == 0){
    tinah.LCD.clear();
    tinah.LCD.print(getControl());
    tinah.LCD.setCursor(0,1);
    tinah.LCD.print((int) derivErr);
    i = 1;
  }
  
  if(startbutton()){
    settingsMenu();
    lastTime = micros();
    intErr = 0;
  }

  i++;
}

//TODO: Actually determine a formula to get the angle
int getAngle() {
  return analogRead(0)/100;
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

//Retrusn the motor speed based on PID control
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

