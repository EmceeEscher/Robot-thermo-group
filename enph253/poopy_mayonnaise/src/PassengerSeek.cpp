///
// PassengerSeek.cpp
//
#include <StandardCplusplus.h>
#include <vector>
#include <phys253.h>
#include "pins.hpp"
#include "PassengerSeek.hpp"


using std::vector;


const int NUM_SAVED_READINGS {20};


// TODO
void PassengerSeek::init()
{
    MinorMode::init();

    std::fill(this->atMax.begin(), this->atMax.end(), false);
    std::fill(this->pinReadings.begin(), this->pinReadings.end(), 0.);

    for (auto &x : this->lastPinReadings)
	std::fill(x.begin(), x.end(), 0.);

    for (auto &x : this->lastPinReadingsDeriv)
	std::fill(x.begin(), x.end(), 0.);
}


// TODO
PassengerSeek::PassengerSeek()
    : MinorMode(),
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
    // Get pin readings
    for (auto i(0); i < this->qsdPinsSides.size(); ++i)
	this->pinReadings[i] = analogRead(this->qsdPinsSides[i]);
    
    // Update QSD readings array
    std::rotate(
            this->lastPinReadings.begin(), this->lastPinReadings.end()-1,
	    this->lastPinReadings.end());
    std::copy(
            this->pinReadings.begin(), this->pinReadings.end(),
	    this->lastPinReadings.front().begin());

    // Update QSD derivative array
    std::rotate(
            this->lastPinReadingsDeriv.begin(),
	    this->lastPinReadingsDeriv.end()-1,
	    this->lastPinReadingsDeriv.end()
    );
    for (auto i(0); i < this->lastPinReadingsDeriv.front().size(); ++i)
	this->lastPinReadingsDeriv.front()[i] =
	    (this->lastPinReadings[0][i] - this->lastPinReadings[1][i]);

    // TODO: If at a maximum, stop tape following
}


// TODO
void PassengerSeek::test()
{
    MinorMode::test();
}


// TODO
bool approachingPassenger()
{
    return false;  // stub
}


// TODO
bool atPassenger()
{
    return false;  // stub
}
