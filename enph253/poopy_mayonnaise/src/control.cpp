///
// control.cpp
//
#include "control.hpp"


namespace control {
    static bool active(false);
}
TapeFollow tf;


void control::loop() {
    if (startbutton() && !control::active) 
	control::start();
    else if (stopbutton() && control::active) 
	control::stop();
    tf.loop();
}

// TODO
void control::start() {
    control::active = true;
    tf.start();
    LCD.clear();
}

void control::stop() {
    control::active = false; 
    tf.stop();
    LCD.clear();
    LCD.print("Press START to");
    LCD.setCursor(0, 1);
    LCD.print("begin");
}
