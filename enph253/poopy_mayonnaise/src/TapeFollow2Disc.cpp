///
// TapeFollow2Disc.cpp
//
#include "TapeFollow2Disc.hpp"
#include <math.h>

//#pragma clang diagnostic push
#pragma ide diagnostic ignored "CannotResolve"

// constants
const double EPSILON(0.01);
const double ERROR_D2F(1.0);
const double ERROR_D1F(3.0);
const double ERROR_D0F(pow(0.5*ERROR_D1F, 2) - EPSILON*ERROR_D1F);
const double PROPL_GAIN(10.0);
const double DERIV_GAIN(10.0);
const int MOTOR_PIN_L(0);      // left motor pin
const int MOTOR_PIN_R(3);      // right motor pin
const int RESET_PERIOD(30);
const int MOTOR_SPEED(200);
const int OVER_TAPE(1);
const int OFF_TAPE(0);

// object constructor
TapeFollow2Disc::TapeFollow2Disc(Tinah &t)
    : errord0F(ERROR_D0F),
      errord1F(ERROR_D1F),
      errord2F(ERROR_D2F),
      gainPropl(PROPL_GAIN),
      gainDeriv(DERIV_GAIN),
      motorPinL(MOTOR_PIN_L),
      motorPinR(MOTOR_PIN_R),
      motorSpeed(MOTOR_SPEED),
      reset(RESET_PERIOD),
      errorPrev(0.0),
      errorRecent(0.0),
      timePrev(0),
      timeNext(0),
      count(0),
      tinah(t)
{
    // initialize tape array with 0's
    for (int row = 0; row < 3; ++row)
	for (int col = 0; col < 4; ++col)
	    this->tape[row][col] = 0;
    // initialize activePins array
    for (int i = 0; i < 4; ++i)
	activePins[i] = i;
}

// main loop function
void TapeFollow2Disc::loop() {
    // declare static variables (runs only once)
    double ctrlPropl(0.0);
    double ctrlDeriv(0.0);
    int control(0);
    
    // get readings from tape sensors
    for (int i(0); i < 4; ++i)
        this->readings[i] = digitalRead(this->activePins[i]);

    // update tape array
    for (int i = 0; i < 4; ++i) {
        this->tape[2][i] = this->tape[1][i];
	    this->tape[1][i] = this->tape[0][i];
        if (this->readings[i] == OVER_TAPE)
            this->tape[0][i] = 1;
        else
            this->tape[0][i] = 0;
    }

    // set the time map
    // left - 0 + right
    for (int i = 0; i < 3; ++i) {
	if (tape[i][1] > tape[i][2])
	    this->timeMap[i] = -1;
	else if (tape[i][1] < tape[i][2])
	    this->timeMap[i] = 1;
	else
	    this->timeMap[i] = 0;
    }

    // determine error from timeMap
    this->errorNext = this->errord0F * this->timeMap[0] +
	this->errord1F * this->timeMap[1] +
	this->errord2F * this->timeMap[2];

    // update errors
    if (this->errorNext != this->errorPrev) {
	    this->errorRecent = this->errorPrev;
	    this->timePrev = this->timeNext;
	    this->timeNext = 1;
    }

    // TODO: Intersection detection

    // get effect of proportional and derivative gains
    ctrlPropl = this->gainPropl * this->errorNext;
    ctrlDeriv = this->gainDeriv *
            (this->errorNext - this->errorRecent) /
            (this->timeNext - this->timePrev);
    control = -static_cast<int>(ctrlPropl + ctrlDeriv);

    // adjust motor speed
    this->tinah.motor.speed(this->motorPinL, (-this->motorSpeed + control));
    this->tinah.motor.speed(this->motorPinR, (this->motorSpeed + control));

    // update counters
    if (this->count == this->reset)
        this->count = 0;
    this->count += 1;
    this->timeNext += 1;
}

//#pragma clang diagnostic pop
