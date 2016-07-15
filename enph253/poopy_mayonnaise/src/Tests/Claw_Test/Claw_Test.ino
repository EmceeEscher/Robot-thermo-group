#include <phys253.h>
#include <phys253pins.h>

const int motorNum = 1;
const int catchSwitch = 0;
const int noCatchSwitch = 1;
const int dropTime = 400; // Milliseconds

void setup() {
  #include <phys253setup.txt>
  defaultMessage();
}

void loop() {
  if(startbutton()){
    grabShit();
    defaultMessage();
  } else if(stopbutton()){
    dropShit();
    defaultMessage();
  }
}

void grabShit(){
  
  if(!digitalRead(catchSwitch) || !digitalRead(noCatchSwitch)){
    LCD.clear();
    LCD.print("Already Grabbing");
    LCD.setCursor(0,1);
    LCD.print("DUMBASS!!!!!!!!!");
    delay(1000);
  } else{
    LCD.clear();
    LCD.print("Grabbing");
    motor.speed(motorNum,140);
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
    motor.speed(motorNum,0);
    delay(1000);
  }
}

void dropShit(){
  LCD.clear();
  LCD.print("Dropping!");
  motor.speed(motorNum,-140);
  delay(dropTime);
  motor.speed(motorNum,0);
}

void defaultMessage(){
  LCD.clear();
  LCD.print("I will obey,");
  LCD.setCursor(0,1);
  LCD.print("         Master.");
}

