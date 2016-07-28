///
// PassengerSeek.cpp
//
#include <phys253.h>
#include "pins.hpp"
#include "myalgs.hpp"
#include "PassengerSeek.hpp"


const int NUM_SAVED_READINGS {24};
const int MAX_REGISTER_PERIOD {10};
const double MAX_REGISTER_THRESHOLD {0.};
const int *PassengerSeek::qsdPinsSides {pins::PASSENGER_SENSORS_SIDES};
const int PassengerSeek::numPinsSides  {pins_sizes::PASSENGER_SENSORS_SIDES};


// TODO
void PassengerSeek::init()
{
    MinorMode::init();

    this->approachingPassenger = false;
    this->atPassenger = false;
    this->passengerSide = 0;  // undefined

    this->atMax.reset();              // reset bits to 000000
    this->lastDerivPositive.reset();  // reset bits to 000000

    for (int i(0); i < PassengerSeek::numPinsSides; ++i) {
	this->pinReadings[i] = 0.;
	this->lastPinReadings[i] = 0.;
	this->numAboveThreshold[i] = 0;
	this->numPosDeriv[i] = 0;
	this->numNegDeriv[i] = 1;
    }
}


bool PassengerSeek::atMaxSideFront()
{
    return this->atMax[2] || this->atMax[3];
}


bool PassengerSeek::atMaxSideMiddle()
{
    return this->atMax[1] || this->atMax[4];
}


// TODO
void PassengerSeek::updateMax()
{
    for (int i(0); i < PassengerSeek::numPinsSides; ++i) {
	bool aboveThreshold = (
                this->numAboveThreshold[i] >= 2*this->maxRegisterPeriod);
	bool imax = (
	        (!this->lastDerivPositive[i]) &&
	        (this->numPosDeriv[i] >= this->maxRegisterPeriod) &&
	        (this->numNegDeriv[i] >= this->maxRegisterPeriod));
	// set array
	this->atMax[i] = static_cast<int>(aboveThreshold && imax);
    }
}


// TODO
PassengerSeek::PassengerSeek()
    : MinorMode(),
      maxRegisterPeriod    (MAX_REGISTER_PERIOD),
      maxRegisterThreshold (MAX_REGISTER_THRESHOLD),
      pinReadings          {0., 0., 0., 0., 0., 0.},
      lastPinReadings      {0., 0., 0., 0., 0., 0.},
      numAboveThreshold    {0, 0, 0, 0, 0, 0},
      numPosDeriv          {0, 0, 0, 0, 0, 0},
      numNegDeriv          {0, 0, 0, 0, 0, 0}
{
    this->init();
}


PassengerSeek::~PassengerSeek() {}


// TODO
void PassengerSeek::loop()
{
    // Get pin readings
    for (int i(0); i < PassengerSeek::numPinsSides; ++i) {
	this->lastPinReadings[i] = this->pinReadings[i];
	this->pinReadings[i] = analogRead(PassengerSeek::qsdPinsSides[i]);
    }
    
    // Update derivative counts
    for (int i(0); i < PassengerSeek::numPinsSides; ++i) {
	if ((this->pinReadings[i] - this->lastPinReadings[i]) <= 0) {
	    if (this->lastDerivPositive[i])
		this->numNegDeriv[i] = 1;
	    else if (this->numNegDeriv[i] < this->maxRegisterPeriod)
		this->numNegDeriv[i] += 1;
	    this->lastDerivPositive[i] = 0;
	} else {
	    if (!this->lastDerivPositive[i])
		this->numPosDeriv[i] = 1;
	    else if (this->numPosDeriv[i] < this->maxRegisterPeriod)
		this->numPosDeriv[i] += 1;
	    this->lastDerivPositive = 1;
	}
    }

    // Update counters
    for (int i(0); i < PassengerSeek::numPinsSides; ++i)
	if (this->pinReadings[i] <= this->maxRegisterThreshold) {
	    this->numAboveThreshold[i] = 0;
	} else if (this->numAboveThreshold[i] < this->maxRegisterPeriod)
	    this->numAboveThreshold[i] += 1;

    // Update atMax array
    this->updateMax();

    // If at a maximum, signal to stop tape following
    if (this->atMaxSideFront())
	this->approachingPassenger = true;
    else if (this->atMaxSideMiddle()) {
	this->approachingPassenger = false;
	this->atPassenger = true;
    }
}


// TODO
void PassengerSeek::test()
{
    MinorMode::test();
}


bool PassengerSeek::isApproachingPassenger()
{
    return this->approachingPassenger;
}


bool PassengerSeek::isAtPassenger()
{
    return this->atPassenger;
}


int PassengerSeek::getPassengerSide()
{
    return this->passengerSide;
}
