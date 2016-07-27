///
// control.cpp
//
#include <phys253.h>
#include "TapeFollow3.hpp"
#include "control.hpp"


namespace control {
    static bool active(false);  // replace this with RobotState instance var
}
TapeFollow3 tf;


void control::setup() {
#include <phys253setup.txt>
    Serial.begin(9600);
    randomSeed(analogRead(0));
    LCD.clear();
    LCD.print("Beefing tears of");
    LCD.setCursor(0, 1);
    LCD.print("soy from my     ");
    delay(500);
    LCD.clear();
    LCD.print("heart of cold   ");
    LCD.setCursor(0, 1);
    LCD.print("aardvark fluids!");
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
