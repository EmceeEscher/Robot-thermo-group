//#pragma clang diagnostic push
#pragma ide diagnostic ignored "CannotResolve"

#include <phys253.h>
#include <LiquidCrystal.h>
#include "TapeFollow.hpp"

Tinah tinah; // initialize Tinah board
TapeFollow tf(tinah);

// functions
void setup() {
    Serial.begin(9600);
    randomSeed(analogRead(0));
    tinah.LCD.clear();
    tinah.LCD.print("ROBLOX 101");
    delay(500);
    tinah.LCD.clear();
    tinah.LCD.print("Are you ready?");
    delay(500);
    tinah.LCD.clear();
    tinah.LCD.print("Press START to begin");
    while (!(startbutton()))
        delay(100);
    tinah.LCD.clear();
}

void loop() {
    tf.loop();
}

//#pragma clang diagnostic pop
