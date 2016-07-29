///
// control.cpp
//
#include <StandardCplusplus.h>
#include <phys253.h>
#include "RobotState.hpp"
#include "control.hpp"


namespace control
{
    static bool active(false);  // replace this with RobotState instance var
}

RobotState s;


void control::setup()
{
    #include <phys253setup.txt>
    RCServo1.detach();
    RCServo2.detach();
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


void control::loop()
{
    if (startbutton() && !control::active) 
	control::start();
    else if (stopbutton() && control::active) 
	control::stop();
    else if (stopbutton())
	control::stop();

    if (s.isActive())
    	s.loop();
}


void control::start()
{
    control::active = true;
    s.start();
    LCD.clear();
}


void control::stop()
{
    control::active = false;
    s.test();
    LCD.clear();
}
