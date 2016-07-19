#include <phys253.h>
#include <phys253pins.h>

void setup() {
  #include <phys253setup.txt>
  RCServo2.detach(); //MUST BE INCLUDED
  pinMode(34,OUTPUT);
}

void loop() {
  /*
   * Alternate Code using max/min PWM values to effectively
   * set digtal Values
   * Probably inefficient.
  analogWrite(34,255);
  delay(2000);
  analogWrite(34,0);
  delay(2000);
  */
  digitalWrite(34,HIGH);
  delay(2000);
  digitalWrite(34,LOW);
  delay(2000);
}
