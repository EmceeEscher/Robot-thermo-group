///
// TapeFollow2Disc.cpp
//
#include "TapeFollow2Disc.h"
#include <math.h>

// constants
const float EPSILON = 0.01;
const float ERROR_D2F = 1.0;
const float ERROR_D1F = 3.0;
const float ERROR_D0F = pow(0.5*ERROR_D1F, 2) - EPSILON*ERROR_D1F;
const float PROPL_GAIN = 10.0;
const float DERIV_GAIN = 10.0;
const float THRESHOLD = 45.0;
const int MOTOR_PIN_L = 0;      // left motor pin
const int MOTOR_PIN_R = 3;      // right motor pin
const int RESET_PERIOD = 30;
const int MOTOR_SPEED = 200;

// object constructor
TapeFollow2Disc::TapeFollow2Disc(Tinah &t)
    : errord0F(ERROR_D0F),
      errord1F(ERROR_D1F),
      errord2F(ERROR_D2F),
      gainPropl(PROPL_GAIN),
      gainDeriv(DERIV_GAIN),
      threshold(THRESHOLD),
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
    // get readings from tape sensors
    this->readings[0] = analogRead(this->activePins[0]);
    this->readings[1] = analogRead(this->activePins[1]);
    this->readings[2] = analogRead(this->activePins[2]);
    this->readings[3] = analogRead(this->activePins[3]);

    // update tape array
    for (int i = 0; i < 4; ++i) {
	this->tape[2][i] = this->tape[1][i];
	this->tape[1][i] = this->tape[0][i];
	if (this->readings[i] > this->threshold)
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
    this->ctrlPropl = this->gainPropl * this->errorNext;
    this->ctrlDeriv = this->gainDeriv *
	(this->errorNext - this->errorRecent) /
	(this->timeNext - this->timePrev);
    this->control = -(this->ctrlPropl + this->ctrlDeriv);

    // adjust motor speed
    this->tinah.motor.speed(this->motorPinL,
			   (int) (-this->motorSpeed + this->control));
    this->tinah.motor.speed(this->motorPinR,
			   (int) (this->motorSpeed + this->control));

    // update counters
    if (this->count == this->reset)
	this->count = 0;
    this->count += 1;
    this->timeNext += 1;
}
