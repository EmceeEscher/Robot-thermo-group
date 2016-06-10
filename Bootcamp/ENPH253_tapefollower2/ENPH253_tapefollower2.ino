#include <phys253.h>          
#include <LiquidCrystal.h>

const int threshold = 45;
const int motorSpeed = 200;
const int printPeriod = 30;

int propGain;
int derivGain;
int leftSensor;
int rightSensor;
int prop;
int deriv;
int control;

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
  LCD.clear();
  LCD.print("Hello world");
}

void loop() {
  propGain = knob(6) / 50;
  derivGain = knob(7) / 50;
  leftSensor = analogRead(2);
  rightSensor = analogRead(0);

  if ((leftSensor > threshold) && (rightSensor > threshold))
    error = 0;
  else if (leftSensor > threshold)
    error = -1;
  else if (rightSensor > threshold)
    error = 1;
  else if (lastError > 0)
      error = 5*2;
  else
      error = -5*2;
  if (error != lastError) {
    recentError = lastError;
    prevTime = timeStep;
    timeStep = 1;
  }

  prop = propGain * error;
  deriv = (int) ((float) derivGain * (float) (error - recentError) /
		 (float) (prevTime - timeStep));
  control = -(prop + deriv);

  if (count == printPeriod) {
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
  motor.speed(0, -motorSpeed + control);
  motor.speed(3, motorSpeed + control);
  lastError = error;
}
