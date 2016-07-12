//#pragma clang diagnostic push
//#pragma ide diagnostic ignored "CannotResolve"


#include <phys253.h>
#include "control.hpp"


// functions
void setup() {
    Serial.begin(9600);
    randomSeed(analogRead(0));
    LCD.clear();
    LCD.print("ROBLOX 101");
    delay(500);
    LCD.clear();
    LCD.print("Are you ready?");
    delay(500);
    LCD.clear();
    LCD.print("Press START to");
    LCD.setCursor(0, 1);
    LCD.print("begin");
}

void loop() {
    control::loop();
}

//#pragma clang diagnostic pop
