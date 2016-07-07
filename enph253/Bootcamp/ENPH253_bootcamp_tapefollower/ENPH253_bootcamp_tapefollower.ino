#include <phys253.h>          
#include <LiquidCrystal.h>

const int threshold = 45;
const int motorSpeed = 200;
int propGain;
int derivGain;

int count = 0;
int error = 0;
int lastError = 0;
int recentError = 0;
int prevTime = 0;
int timeStep = 0;


void setup() {
  // put your setup code here, to run once:
  #include <phys253setup.txt>
  Serial.begin(9600);
}

void loop(){
  propGain = knob(6) / 50;
  derivGain = knob(7) / 50;
  
  int leftSensor = analogRead(2);
  int rightSensor = analogRead(0);

  if( (leftSensor > threshold) && (rightSensor > threshold) )
    error = 0;
  else if( (leftSensor > threshold) && (rightSensor < threshold) )
    error = -1;
  else if( (leftSensor < threshold) && (rightSensor > threshold) )
    error = 1;
  else if( (leftSensor < threshold) && (rightSensor < threshold) ){
    if( lastError > 0 ){
      error = 5;
    }
    else if (lastError <= 0){
      error = -5;
    }
  }

  if( !(error == lastError) ){
    recentError = lastError;
    prevTime = timeStep;
    timeStep = 1;
  }

  int prop = propGain * error;
  int deriv = (int) ((float) derivGain * (float) (error - recentError) / (float) (prevTime - timeStep));
  int control = -1*(prop + deriv);

  if(count == 30){
    LCD.clear();
    LCD.print(rightSensor);
    LCD.print(" ");
    LCD.print(leftSensor);
    LCD.print(" ");
    LCD.print(propGain);
    LCD.print(" ");
    LCD.print(derivGain);
    LCD.print(" ");
    LCD.print(prop);
    LCD.print(" ");
    LCD.print(deriv);
    LCD.print(" ");
    
    count = 0;
  }

  count = count + 1;
  timeStep = timeStep + 1;
  motor.speed(0, -1 * motorSpeed + control);
  motor.speed(3, motorSpeed + control);
  lastError = error;
}
