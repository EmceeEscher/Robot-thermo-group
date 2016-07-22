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

    fill(this->atMax.begin(), this->atMax.end(), false);
    fill(this->pinReadings.begin(), this->pinReadings.end(), 0.);

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
    for (unsigned int i(0); i < this->pinReadings.size(); ++i) {
	// determine if all above threshold
	bool allAboveThreshold = true;
	for (auto j(0); j < 2 * this->maxRegisterPeriod; ++j)
	    if (this->lastPinReadings[j][i] <= this->maxRegisterThreshold) {
		allAboveThreshold = false;
		break;
	    }
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
	this->atMax[i] = allAboveThreshold && imax;
    }
}


// TODO
PassengerSeek::PassengerSeek()
    : MinorMode(),
      maxRegisterPeriod    (MAX_REGISTER_PERIOD),
      maxRegisterThreshold (MAX_REGISTER_THRESHOLD),
      qsdPinsSides         (pins::PASSENGER_SENSORS_SIDES),
      atMax                (6, false),
      pinReadings          (6, 0.),
      lastPinReadings      (NUM_SAVED_READINGS,   vector<double>(6, 0.)),
      lastPinReadingsDeriv (NUM_SAVED_READINGS-1, vector<double>(6, 0.))
{
}


PassengerSeek::~PassengerSeek() {}


// TODO
void PassengerSeek::loop()
{
    LCD.clear();
    // Get pin readings
    LCD.print("0");
    delay(500);
    for (int i(0); i < 6; ++i)
	this->pinReadings[i] = 1.;  // TODO: fixme
	// this->pinReadings[i] = analogRead(this->qsdPinsSides[i]);
    
    // Update QSD readings array
    LCD.print("1");
    delay(500);
    std::rotate(
            this->lastPinReadings.begin(), this->lastPinReadings.end()-1,
	    this->lastPinReadings.end()
    );
    LCD.print("2");
    delay(500);
    std::copy(
            this->pinReadings.begin(), this->pinReadings.end(),
    	    this->lastPinReadings.front().begin()
    );

    // Update QSD derivative array
    LCD.print("3");
    delay(500);
    std::rotate(
            this->lastPinReadingsDeriv.begin(),
    	    this->lastPinReadingsDeriv.end()-1,
    	    this->lastPinReadingsDeriv.end()
    );
    LCD.print("4");
    delay(500);
    for (auto i(0); i < 6; ++i)
	this->lastPinReadingsDeriv.front()[i] =
	    (this->lastPinReadings[0][i] - this->lastPinReadings[1][i]);

    // Update atMax array
    LCD.print("5");
    delay(500);
    this->updateMax();

    // If at a maximum, signal to stop tape following
    LCD.print("6");
    delay(500);
    if (this->atMaxSideFront())
	this->approachingPassenger = true;
    else if (this->atMaxSideMiddle()) {
	this->approachingPassenger = false;
	this->atPassenger = true;
    }

    LCD.print("7");
    delay(1000);
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
