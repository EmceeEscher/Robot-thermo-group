#include <phys253.h>

Tinah tinah;  // setup of tinah objects happens here

// use tinah.X to access objects:
//   LCD
//   motor
//   RCServo0
//   RCServo1
//   RCServo2

void setup() {
    Serial.begin(9600);
    randomSeed(analogRead(0));
    tinah.LCD.clear();
    tinah.LCD.print("Hello world");
}

void loop() {
    // loop control code here
}
