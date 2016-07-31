#include <phys253.h>
#include <phys253pins.h>


//Amount of time to run motor to drop animal
const int dropTime = 400; // Milliseconds

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
const float BASE_REST_POSITION = 90;
const float MID_REST_POSITION = 140;
const float BASE_HOLD_POSITION = 90;
const float MID_HOLD_POSITION = 170;

//Iterator for LCD printing
int LCDControl;

//Stepper Constants
const int COUNTERCLOCKWISE = HIGH;
const int CLOCKWISE = LOW;
const int stepperMicrosDelay = 1600; //Time delay between pulses in microseconds
const int numPulses = 680;

//reachAndGrab/reachAndDrop function Constants
const float initialAdjMidTarget = 100;
const float initialAdjBaseTarget = 60;
const float midAdjMidTarget = 135;
const float midAdjBaseTarget = 40;
const float finalAdjMidTarget = 170;
const float finalAdjBaseTarget = 20;

//Holding a passenger?
bool holding = false;

/*
void setup() {
  #include <phys253setup.txt>
  portMode(1,OUTPUT);
  pinMode(DIR_PIN,OUTPUT);
  pinMode(PULSE_PIN,OUTPUT);
  Serial.begin(9600);
  baseTarget = BASE_REST_POSITION;
  midTarget = MID_REST_POSITION;
  lastPropErr = 0.;
  LCDControl = 1;
}
*/

/*
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
*/

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
  setBaseMotor((long) getControlValue());

  RCServo0.write(midTarget);
  
  lastTime = now;
  lastPropErr = propErr;

  
  if(LCDControl % 25 == 0){
    //printState();
    LCDControl = 1;
  }
  LCDControl++;
}

//Converts base potentiometer voltage to corresponding angle
float getAngle() {
  float voltage = (float) analogRead(POTENTIOMETER) * 5./1024.;
  return 130.814 * (3. * voltage - 10.) / (voltage - 5.) + 60.;
}

//Wrapper function for setting motor speed
//Prevents values larger than 255 in either direction
void setBaseMotor(long duty){
  if(duty > 255){
    duty = 255;
  }
  else if(duty < -255){
    duty = -255;
  }
  motor.speed(MOTOR_PIN_ARM,duty);
}

//Returns the motor speed based on PID control
float getControlValue(){
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
void grabShit(){

  //If switches are already triggered, then do nothing
  if(!digitalRead(ARM_SWITCHES[0]) || !digitalRead(ARM_SWITCHES[1])){}
  
  else{
    LCD.clear();
    LCD.print("Grabbing");
    motor.speed(MOTOR_PIN_BABY,190);
    unsigned long startTime = millis();
    while(1){
      doControl();
      if(!digitalRead(ARM_SWITCHES[0])){
        holding = true;
        break;
      }
      if(!digitalRead(ARM_SWITCHES[1])){
        holding = false;
        break;
      }
      if(millis() - startTime > 2000){
        holding = true;
        break;
      }
    }
    if(holding){
      motor.speed(MOTOR_PIN_BABY,20);
    }else{
      dropShit();
    }
  }
}

//Opens the claw for specified time
void dropShit(){
  motor.speed(MOTOR_PIN_BABY,-140);
  holding = false;
  delay(dropTime);
  motor.speed(MOTOR_PIN_BABY,0);
}

/*
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
  LCD.print(holding);
}
*/

/*
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
    if(!digitalRead(ARM_SWITCHES[2])){
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
*/

void reachAndClaw(bool grabbing)
{
    midTarget = initialAdjMidTarget;
    baseTarget = initialAdjBaseTarget;
    unsigned long startTime = millis();
    while (millis() - startTime < 500) 
  doControl();
    
    midTarget = midAdjMidTarget;
    baseTarget = midAdjBaseTarget;
    
    while (millis() - startTime < 1000) 
  doControl();
    
    baseTarget = finalAdjBaseTarget; 
    midTarget = finalAdjMidTarget;
    while (millis() - startTime < 1500) {  
  doControl();
  if (!digitalRead(ARM_SWITCHES[2])) {
      baseTarget = getAngle() + 5;
      break;
  } 
    }
    if (grabbing) 
  grabShit();
    else 
  dropShit();
    setRestPosition();
}

/*
//Sets the control target values to rest position
void setRestPosition(){
  if(holding){
    baseTarget = BASE_HOLD_POSITION;
    midTarget = MID_HOLD_POSITION;
  }else{
    baseTarget = BASE_REST_POSITION;
    midTarget = MID_REST_POSITION;
  }
  
}
*/

void setRestPosition(){
    unsigned long startTime = millis();
    baseTarget = midAdjBaseTarget; 
    midTarget = midAdjMidTarget;
    while (millis() - startTime < 500) 
  doControl();
    
    baseTarget = initialAdjBaseTarget;
    midTarget = initialAdjMidTarget;
    while (millis() - startTime < 1000)  // TODO: no hard coding
  doControl();
    
    if (holding) {
  baseTarget = BASE_HOLD_POSITION;
  midTarget = MID_HOLD_POSITION;
    } else {
  baseTarget = BASE_REST_POSITION;
  midTarget = MID_REST_POSITION;
    }
}

//Turns the stepper motor a specified number of steps
void stepperTurn(bool CW,int count){
  if(CW){
    digitalWrite(DIR_PIN,CLOCKWISE);
  } else{
    digitalWrite(DIR_PIN,COUNTERCLOCKWISE);
  }
  int i;
  unsigned long prevTime = millis();
  for(i = 0; i < count; i++){
    if(millis() - prevTime > 10){
      doControl();
      prevTime = millis();
    }
    digitalWrite(PULSE_PIN,HIGH);
    delayMicroseconds(stepperMicrosDelay);
    
    digitalWrite(PULSE_PIN,LOW);
    delayMicroseconds(stepperMicrosDelay);
  }
}

/* 
 * Parameter: turnRight - Turn right if true, left otherwise
 * Parameter: grab - grab if true, drop otherwise
 */
void turnAndReach(bool turnRight, bool grab){
  stepperTurn(turnRight, numPulses);
  reachAndClaw(grab);
  stepperTurn(!turnRight, numPulses);
}

