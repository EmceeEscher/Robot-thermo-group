//#pragma clang diagnostic push
//#pragma ide diagnostic ignored "CannotResolve"

#include <phys253.h>
#include <LiquidCrystal.h>
#include "TapeFollow.hpp"

bool active(false);
TapeFollow tf;

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
    /* while (!(startbutton())) */
    /* 	delay(100); */
    /* LCD.clear(); */
}

void loop() {
    if (startbutton() && !active) {
	active = true;
	tf.start();
	LCD.clear();
    } else if (stopbutton() && active) {
	active = false;
	tf.stop();
	LCD.clear();
	LCD.print("Press START to");
	LCD.setCursor(0, 1);
	LCD.print("begin");
    }

    tf.loop();
}

//#pragma clang diagnostic pop
