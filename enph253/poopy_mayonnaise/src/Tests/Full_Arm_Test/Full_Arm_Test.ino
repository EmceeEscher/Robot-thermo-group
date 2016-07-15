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

float propGain, derivGain, intGain;
float baseTarget, midTarget;
float propErr, derivErr, intErr, lastPropErr;
float angle;
float now, lastTime;

//Iterator for control parameters
int j;
const int PROP = 0, INT = 1, DERIV = 2;//Potential values

//Iterator for LCD output
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
  j = PROP;
}

void loop() {
  angle = getAngle();

  baseTarget = map(knob(6),0,1024,10,80);
  midTarget = map(knob(7),0,1024,0,180);

  now = micros();
  propErr = angle - baseTarget;
  derivErr = (propErr - lastPropErr)/(now - lastTime) * 1000000.;
  intErr += (propErr + lastPropErr)/2 * (now - lastTime) / 1000000.;
  setBaseMotor((int) getControl());

  RCServo0.write(midTarget);
  
  lastTime = now;
  lastPropErr = propErr;

  if(i % 25 == 0){
    printState();
    i = 1;
  }

  if(startbutton()){
    grabShit();
  } else if (stopbutton()){
    dropShit;
  }

  if(Serial.available() > 0){
    switch(j){
      case PROP:
      propGain = Serial.parseFloat();
      j = INT;
      break;
      case INT:
      intGain = Serial.parseFloat();
      j = DERIV;
      break;
      case DERIV:
      derivGain = Serial.parseFloat();
      j = PROP;
      break;
    }
  }

  i++;
  delay(10);
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
  motor.speed(baseMotorNumber,duty);
}

//Returns the motor speed based on PID control
float getControl(){
  float control = 0;
  control += propGain * propErr;
  control += derivGain * derivErr;
  control += intGain * intErr;
  return control;
}

void grabShit(){
  
  if(!digitalRead(catchSwitch) || !digitalRead(noCatchSwitch)){
    LCD.clear();
    LCD.print("Already Grabbing");
    LCD.setCursor(0,1);
    LCD.print("DUMBASS!!!!!!!!!");
    delay(400);
  } else{
    LCD.clear();
    LCD.print("Grabbing");
    motor.speed(babyMotorNum,140);
    while(1){
      if(!digitalRead(catchSwitch)){
        LCD.clear();
        LCD.print("Gotcha!");
        break;
      }
      if(!digitalRead(noCatchSwitch)){
        LCD.clear();
        LCD.print("DAMNIT!");
        break;
      }
    }
    motor.speed(babyMotorNum,0);
    delay(300);
  }
}

void dropShit(){
  LCD.clear();
  LCD.print("Dropping!");
  motor.speed(babyMotorNum,-140);
  delay(dropTime);
  motor.speed(babyMotorNum,0);
}

bool k = false;

void printState(){
  LCD.clear();
  LCD.print("C");
  LCD.print(getControl());
  LCD.print("B");
  LCD.print(baseTarget);
  LCD.print("M");
  LCD.print(midTarget);
  LCD.setCursor(0,1);
  if(j == PROP){
    if(k){
      LCD.print("P");
    } else{
      LCD.print(" ");
    }
  } else{
    LCD.print("P");
  }
  LCD.print(propGain);

  if(j == INT){
    if(k){
      LCD.print("I");
    } else{
      LCD.print(" ");
    }
  } else{
    LCD.print("I");
  }
  LCD.print(intGain);

  if(j == DERIV){
    if(k){
      LCD.print("D");
    } else{
      LCD.print(" ");
    }
  } else{
    LCD.print("D");
  }
  LCD.print(derivGain);
  k = !k;
}

