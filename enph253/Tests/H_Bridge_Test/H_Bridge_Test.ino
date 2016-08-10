#include <phys253.h>
#include <phys253pins.h>

const int motorNum = 0;
int pwm;

void setup() {
  #include <phys253setup.txt>

}

void loop() {
  pwm = map(knob(6),0,1024,-255,255);
  motor.speed(motorNum,pwm);
  LCD.clear();
  LCD.print(pwm);
  delay(200);
}
