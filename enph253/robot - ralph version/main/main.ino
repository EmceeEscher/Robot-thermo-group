#include <phys253.h>

bool started = false;

void setup() {
  // put your setup code here, to run once:
  #include <phys253setup.txt>
  RCServo1.detach();
  RCServo2.detach();
  Serial.begin(9600);
  randomSeed(analogRead(0));
  LCD.clear();
  LCD.print("Press START to");
  LCD.setCursor(0, 1);
  LCD.print("begin");
}

void loop() {
  if(startbutton()&&!started){
    started = true;
  }
  else if(stopbutton()&&started){
    started = false;
  }
  if(started){
    //do everything
  }

}
