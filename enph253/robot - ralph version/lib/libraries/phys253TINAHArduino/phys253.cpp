#include <Arduino.h>
#include <motor.h>
#include <LiquidCrystal.h>
#include <motor.h>
#include <ServoTimer2.h>
#include <phys253pins.h>
#include <phys253.h>


//setup the variables and classes used throughout phys 253.
LiquidCrystal LCD(26,27,28,16,17,18,19,20,21,22,23);
motorClass motor ;
ServoTimer2 RCServo0;    // declare variables for up to eight servos.   Replaced old Servo253 implementation 2015Jan2
ServoTimer2 RCServo1; 
ServoTimer2 RCServo2;


int knob(int value)
{
    return analogRead(knobInput[value]);
}


void buzzer(int value)
{
    return;  //   pulseOut(buzzerOutput, value*2) ;}
} 


void buzzerOff()
{
    return; //   pulseStop(buzzerOutput ) ;}
}


int startbutton()
{
    return !digitalRead(startInput);
}


int stopbutton()
{
    return !digitalRead(stopInput);
}


void portMode(int portval, int value)
{
    for (int tempval(0); tempval < 8; ++tempval)  
	pinMode(portval*8 + tempval, value);
}


void portWrite(int portval, int value)
{
    for (int tempval(0); tempval < 8; ++tempval)  
	digitalWrite(portval*8 + tempval, value & tempval);
}


int  portRead(int portval) {
    int tempTotal(0);
    int tempRead(0);
    int interimVal(1);
    int tempState;
    for (int tempval(0); tempval < 8; ++tempval) {
	tempRead = digitalRead(portval*8 + tempval);
	if (tempRead == HIGH)
	    tempState = 1;
	else
	    tempState = 0;
	//  Serial.print(tempState) ;
	
	tempTotal = tempTotal + tempState * interimVal;
	Serial.print(' ');
	Serial.print(interimVal);
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
