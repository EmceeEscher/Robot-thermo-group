///
// PassengerSeek.cpp
//
#include <StandardCplusplus.h>
#include <algorithm>
#include <vector>
#include <phys253.h>
#include "pins.hpp"
#include "myalgs.hpp"
#include "PassengerSeek.hpp"


using std::vector;


const int NUM_SAVED_READINGS {24};
const int MAX_REGISTER_PERIOD {10};
const double MAX_REGISTER_THRESHOLD {0.};


// TODO
void PassengerSeek::init()
{
    MinorMode::init();

    using std::fill;

    this->approachingPassenger = false;
    this->atPassenger = false;
    this->passengerSide = 0;  // undefined

    this->atMax.reset();  // reset bits to 000000

    for (int i(0); i < 6; ++i) {
	this->pinReadings[i] = 0.;
	this->lastTimePinReadings[i] = 0.;
	this->numAboveThreshold[i] = 0;
    }

    for (auto &x : this->lastPinReadings)
	fill(x.begin(), x.end(), 0.);

    for (auto &x : this->lastPinReadingsDeriv)
	fill(x.begin(), x.end(), 0.);
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
    for (int i(0); i < 6; ++i) {
	bool aboveThreshold =
	    (this->numAboveThreshold[i]) > (this->maxRegisterPeriod);
	// determine if at a max
	bool imax = true;
	for (auto j(0); j < this->maxRegisterPeriod; ++j) {
	    if (this->lastPinReadingsDeriv[j][i] >= 0) {
		imax = false;
		break;
	    }
	    if (this->lastPinReadingsDeriv[j+this->maxRegisterPeriod][i] <= 0) {
		imax = false;
		break;
	    }
	}
	// set array
	this->atMax[i] = aboveThreshold && imax;
    }
}


// TODO
PassengerSeek::PassengerSeek()
    : MinorMode(),
      maxRegisterPeriod    (MAX_REGISTER_PERIOD),
      maxRegisterThreshold (MAX_REGISTER_THRESHOLD),
      qsdPinsSides         (pins::PASSENGER_SENSORS_SIDES),
      pinReadings          {0., 0., 0., 0., 0., 0.},
      lastPinReadings      (NUM_SAVED_READINGS,   vector<float>(6, 0.)),
      lastPinReadingsDeriv (NUM_SAVED_READINGS-1, vector<float>(6, 0.))
{
}


PassengerSeek::~PassengerSeek() {}


// TODO
void PassengerSeek::loop()
{
    // Get pin readings
    for (int i(0); i < 6; ++i) {
	this->lastTimePinReadings[i] = this->pinReadings[i];
	this->pinReadings[i] = analogRead(this->qsdPinsSides[i]);
    }
    
    // Update QSD readings array
    std::rotate(
            this->lastPinReadings.begin(), this->lastPinReadings.end()-1,
	    this->lastPinReadings.end()
    );
    for (int i(0); i < 6; ++i)
	this->lastPinReadings.front()[i] = this->pinReadings[i];

    // Update QSD derivative array
    std::rotate(
            this->lastPinReadingsDeriv.begin(),
    	    this->lastPinReadingsDeriv.end()-1,
    	    this->lastPinReadingsDeriv.end()
    );
    for (auto i(0); i < 6; ++i)
	this->lastPinReadingsDeriv.front()[i] =
	    (this->lastPinReadings[0][i] - this->lastPinReadings[1][i]);

    // Update counters
    for (int i(0); i < 6; ++i)
	if (this->pinReadings[i] > this->maxRegisterThreshold) {
	    if (this->numAboveThreshold[i] < this->maxRegisterPeriod)
		this->numAboveThreshold[i] += 1;
	} else
	    this->numAboveThreshold[i] = 0;

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
