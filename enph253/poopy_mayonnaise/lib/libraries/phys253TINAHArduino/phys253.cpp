#include <Arduino.h>
#include <LiquidCrystal.h>
#include <motor.h>
#include <phys253pins.h>
//#include <Tone.cpp>
#include <ServoTimer2.h>
#include <phys253.h>


static TinahObjects  &t = TinahObjects::getInstance();  // internal
extern LiquidCrystal &LCD      = t.LCD;                 // external
extern motorClass    &motor    = t.motor;               // external
extern ServoTimer2   &RCServo0 = t.RCServo0;            // external
extern ServoTimer2   &RCServo1 = t.RCServo1;            // external
extern ServoTimer2   &RCServo2 = t.RCServo2;            // external


// Tinah methods
TinahObjects::TinahObjects()
    : LCD(26,27,28,16,17,18,19,20,21,22,23)
{
    //setup the variables and classes used throughout phys 253.
    portMode(0, INPUT);      //   ***** from 253 template file
    portMode(1, INPUT);      //   ***** from 253 template file
    this->LCD.begin(16,2);

    this->RCServo0.attach(RCServo0Output);
    this->RCServo1.attach(RCServo1Output);
    this->RCServo2.attach(RCServo2Output);
}


TinahObjects& TinahObjects::getInstance() {
    static TinahObjects t;  // runs only once
    return t;
}


// functions
int knob(int value) {return analogRead(knobInput[value]);}
void buzzer(int value) {return;} //   pulseOut(buzzerOutput, value*2) ;}
void buzzerOff() {return;} //   pulseStop(buzzerOutput ) ;}
int startbutton() {return !digitalRead(startInput);}
int stopbutton() {return !digitalRead(stopInput);}

void portMode(int portval, int value) {
    for (int tempval = 0; tempval < 8; tempval++)
	pinMode(portval*8 + tempval, value);
}

void portWrite(int portval, int value) {
    for (int tempval = 0; tempval < 8; tempval++)
	digitalWrite(portval*8 + tempval, value & tempval);
}

int  portRead(int portval) {
    int tempTotal = 0;
    int tempRead = 0;
    int interimVal = 1;
    int tempState;
    for (int tempval = 0; tempval < 8; tempval++)  {
	tempRead = digitalRead(portval*8 + tempval);
	if (tempRead == HIGH)
	    tempState = 1;
	else
	    tempState = 0;
	//  Serial.print(tempState) ;
	tempTotal = tempTotal + tempState * interimVal;
	Serial.print(' ');
	Serial.print (interimVal);
	interimVal = interimVal * 2; 
	//     Serial.print(' ') ;
	//     Serial.print(tempTotal) ;
    }
    Serial.println(' ');
    return tempTotal;
}

void analogWriteReset(int PWMPin)   // added 2009 May 18th - required to disable analogWrite on PWM lines
{
    switch(PWMPin) {
    case 5:
    case 29:
	timer1PWMAOff();
	break ;
    case 4:
    case 30:
	timer1PWMBOff();
	break ;
    case 3:
    case 31:
	timer1PWMCOff();
	break ;
    case 2:
    case 35:
	timer3PWMAOff();
	break ;
    case 1:
    case 36:
	timer3PWMBOff();
	break ;
    case 0:
    case 37:
	timer3PWMCOff();
	break ;
    }
}

