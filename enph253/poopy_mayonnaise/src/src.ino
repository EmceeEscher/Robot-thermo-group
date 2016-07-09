#include <phys253.h>
#include <LiquidCrystal.h>
#include "TapeFollow.h"

Tinah tinah; // initialize Tinah board
TapeFollow tf(tinah);

// functions
void setup() {
    Serial.begin(9600);
    randomSeed(analogRead(0));
    tinah.LCD.clear();
    tinah.LCD.print("POOPY MAYONNAISE");
    delay(500);
    tinah.LCD.clear();
    tinah.LCD.print("It's got that\nnice ring to it.");
    delay(500);
    tinah.LCD.clear();
}

void loop() {
    tf.loop();
}
