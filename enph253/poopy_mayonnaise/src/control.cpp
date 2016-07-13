///
// control.cpp
//
#include "control.hpp"


namespace control {
    static bool active(false);
}
TapeFollow3 tf;


void control::setup() {
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

void control::loop() {
    if (startbutton() && !control::active) 
	control::start();
    else if (stopbutton() && control::active) 
	control::stop();
    else if (stopbutton())
	control::stop();
    tf.loop();
}

void control::start() {
    control::active = true;
    tf.start();
    LCD.clear();
}

void control::stop() {
    control::active = false;
    tf.test();
    LCD.clear();
}
