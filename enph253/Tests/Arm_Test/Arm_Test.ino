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

int i;

void setup() {
  #include <phys253setup.txt>
  Serial.begin(9600);
  propGain = 0.; 
  derivGain = 0.; 
  intGain = 0.;
  baseTarget = 45.;
  midTarget = 0;
  lastPropErr = 0;
  RCServo0.write(midTarget);
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

  if(i % 25 == 0){
    LCD.clear();
    LCD.print(getControl());
    LCD.setCursor(0,1);
    LCD.print((int) getAngle());
    i = 1;
  }
  
  if(stopbutton()){
    serialMenu();
    lastTime = micros();
    intErr = 0;
    RCServo0.write(midTarget);
  }

  i++;
  delay(10);
}

//TODO: Actually determine a formula to get the angle
float getAngle() {
  float voltage = (float) analogRead(baseAnglePin) * 5./1024.;
  return 9.*((-142.857 * voltage) / (voltage - 5.));
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
  motor.speed(0,duty);
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
  
  LCD.clear();
  LCD.print("Settings");
  while(startbutton()){}// Wait until button is released

  LCD.clear();
  LCD.print("P: ");
  LCD.print(propGain);
  LCD.setCursor(0,1);
  LCD.print("Orig: ");
  LCD.print(origPropGain);
  while(1){
    propGain = (float) knob(6) / 200; //Determine new propGain
    if(propGain != lastPropGain){
      LCD.clear();
      LCD.print("P: ");
      LCD.print(propGain);
      LCD.setCursor(0,1);
      LCD.print("Orig: ");
      LCD.print(origPropGain);

      lastPropGain = propGain;
    }
    if(startbutton()){
      break;
    }
  }
  LCD.clear();
  while(startbutton()){}

  LCD.print("I: ");
  LCD.print(intGain);
  LCD.setCursor(0,1);
  LCD.print("Orig: ");
  LCD.print(origIntGain);
  while(1){
    intGain = (float) knob(6) / 200; //Determine new intGain
    if(intGain != lastIntGain){
      LCD.clear();
      LCD.print("I: ");
      LCD.print(intGain);
      LCD.setCursor(0,1);
      LCD.print("Orig: ");
      LCD.print(origIntGain);

      lastIntGain = intGain;
    }
    if(startbutton()){
      break;
    }
  }
  LCD.clear();
  while(startbutton()){}

  LCD.print("D: ");
  LCD.print(derivGain);
  LCD.setCursor(0,1);
  LCD.print("Orig: ");
  LCD.print(origDerivGain);
  while(1){
    derivGain = (float) knob(6) / 200; //Determine new derivGain
    if(derivGain != lastDerivGain){
      LCD.clear();
      LCD.print("D: ");
      LCD.print(derivGain);
      LCD.setCursor(0,1);
      LCD.print("Orig: ");
      LCD.print(origDerivGain);

      lastDerivGain = derivGain;
    }
    if(startbutton()){
      break;
    }
  }
  LCD.clear();
  while(startbutton()){}

}

void serialMenu(){

  LCD.clear();
  LCD.print("Serial Menu");
  delay(800);

  //Proportional Gain
  LCD.clear();
  LCD.print("P:");
  LCD.setCursor(0,1);
  LCD.print("Orig: ");
  LCD.print(propGain);
  while(1){
    if(Serial.available() > 0){
      propGain = Serial.parseFloat();
      break;
    }
    else{
      delay(1);
    }
  }
  LCD.clear();
  LCD.print("P: ");
  LCD.print(propGain);
  delay(800);

  //Integral Gain
  LCD.clear();
  LCD.print("I:");
  LCD.setCursor(0,1);
  LCD.print("Orig: ");
  LCD.print(intGain);
  while(1){
    if(Serial.available() > 0){
      intGain = Serial.parseFloat();
      break;
    }
    else{
      delay(1);
    }
  }
  LCD.clear();
  LCD.print("I: ");
  LCD.print(intGain);
  delay(800);

  //Derivative Gain
  LCD.clear();
  LCD.print("D:");
  LCD.setCursor(0,1);
  LCD.print("Orig: ");
  LCD.print(derivGain);
  while(1){
    if(Serial.available() > 0){
      derivGain = Serial.parseFloat();
      break;
    }
    else{
      delay(1);
    }
  }
  LCD.clear();
  LCD.print("D: ");
  LCD.print(derivGain);
  delay(800);

  //Base Target
  LCD.clear();
  LCD.print("Base:");
  LCD.setCursor(0,1);
  LCD.print("Orig: ");
  LCD.print(baseTarget);
  while(1){
    if(Serial.available() > 0){
      baseTarget = Serial.parseFloat();
      break;
    }
    else{
      delay(1);
    }
  }
  LCD.clear();
  LCD.print("Base: ");
  LCD.print(baseTarget);
  delay(800);

  //Middle Joint Target
  LCD.clear();
  LCD.print("Mid:");
  LCD.setCursor(0,1);
  LCD.print("Orig: ");
  LCD.print(midTarget);
  while(1){
    if(Serial.available() > 0){
      midTarget = Serial.parseFloat();
      break;
    }
    else{
      delay(1);
    }
  }
  LCD.clear();
  LCD.print("Mid: ");
  LCD.print(midTarget);
  delay(800);
  
}
