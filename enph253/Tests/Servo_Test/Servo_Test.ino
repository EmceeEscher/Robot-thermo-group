#include <phys253.h>
#include <phys253pins.h>


void setup() {
  // put your setup code here, to run once:
  #include <phys253setup.txt>
}

void loop() {
  // put your main code here, to run repeatedly:
  RCServo0.write((((float)knob(7))/1024 * 270));
  LCD.clear();
  LCD.print((((float)knob(7))/1024 * 270));
  delay(50);
}
