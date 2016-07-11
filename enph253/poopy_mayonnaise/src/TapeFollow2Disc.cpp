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
      count(0),
      tinah(t)
{
    // initialize tape array with 0's
    for (int row = 0; row < 3; ++row)
        for (int col = 0; col < 4; ++col)
            this->tape[row][col] = false;
    // initialize activePins array
    for (int i = 0; i < 4; ++i)
        activePins[i] = i;
}

// main loop function
void TapeFollow2Disc::loop() {
    // declare static variables (runs only once)
    static double error(0.0);
    static double ctrlPropl(0.0);
    static double ctrlDeriv(0.0);
    static int time(0);
    static int control(0);
    static bool readings[4];
    
    // get readings from tape sensors
    for (int i(0); i < 4; ++i)
        readings[i] = static_cast<bool>(digitalRead(this->activePins[i]));

    // update tape array
    for (int i = 0; i < 4; ++i) {
        this->tape[2][i] = this->tape[1][i];
	    this->tape[1][i] = this->tape[0][i];
        this->tape[0][i] = readings[i];
    }

    // set the time map
    // left - 0 + right
    for (int i = 0; i < 3; ++i) {
	    if (tape[i][1] > tape[i][2])
	        this->timePositionMap[i] = -1;
	    else if (tape[i][1] < tape[i][2])
	        this->timePositionMap[i] = 1;
	    else
	        this->timePositionMap[i] = 0;
    }

    // determine error from timeMap
    error = this->errord0F * this->timePositionMap[0] +
	this->errord1F * this->timePositionMap[1] +
	this->errord2F * this->timePositionMap[2];

    // update errors
    if (error != this->errorPrev) {
	    this->errorRecent = this->errorPrev;
	    this->timePrev = time;
	    time = 1;
    }

    // TODO: Intersection detection

    // get effect of proportional and derivative gains
    ctrlPropl = this->gainPropl * error;
    ctrlDeriv = this->gainDeriv *
            (error - this->errorRecent) / (time - this->timePrev);
    control = -static_cast<int>(ctrlPropl + ctrlDeriv);

    // adjust motor speed
    this->tinah.motor.speed(this->motorPinL, (-this->motorSpeed + control));
    this->tinah.motor.speed(this->motorPinR, (this->motorSpeed + control));

    // update counters
    if (this->count == this->reset)
        this->count = 0;
    this->count += 1;
    time += 1;
}

//#pragma clang diagnostic pop
