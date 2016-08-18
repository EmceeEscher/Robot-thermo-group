#ifndef phys253_h
#define phys253_h


// include all of the h-file headers used in general Phys 253 programs
// added by Jon Nakane, 2009 March 23
// edit 2010 May 3 - removed WConstants.h from here and motor.h for Wiring-0026.

#include <Arduino.h>
#include <phys253pins.h>
#include <LiquidCrystal.h>
#include <motor.h>
/* #include <Tone.cpp> */
#include <ServoTimer2.h>

extern LiquidCrystal LCD;
extern motorClass    motor;
extern ServoTimer2   RCServo0;
extern ServoTimer2   RCServo1;
extern ServoTimer2   RCServo2;

int knob(int value);	//	{ return analogRead(knobInput[value]) ;}
void buzzer(int value);//{ return pulseOut(buzzerOutput, value*2) ;}
void buzzerOff();//{ return pulseStop(buzzerOutput ) ;}
int startbutton();	//{ return !digitalRead(startInput) ;}
int stopbutton();	//{ return !digitalRead(stopInput) ;}
void portMode(int portval, int value);
void portWrite(int portval, int value);
int portRead(int portval);


#ifdef __cplusplus            // added 2009 May 18th - required to disable analogWrite on PWM lines.
extern "C" {
#endif
void timer1PWMAOff(void);
void timer1PWMBOff(void);
void timer1PWMCOff(void);
void timer3PWMAOff(void);
void timer3PWMBOff(void);
void timer3PWMCOff(void);
#ifdef __cplusplus
}
#endif


void analogWriteReset(int PWMPin);

#endif
