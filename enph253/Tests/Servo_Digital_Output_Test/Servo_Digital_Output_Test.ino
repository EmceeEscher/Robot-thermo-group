#include <phys253.h>
#include <phys253pins.h>

void setup() {
  #include <phys253setup.txt>
  RCServo1.detach(); //MUST BE INCLUDED
  pinMode(31,OUTPUT);
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
  while(stopbutton()){
    digitalWrite(31,HIGH);
    delay(10);
    digitalWrite(31,LOW);
    delay(10);
  }
}
