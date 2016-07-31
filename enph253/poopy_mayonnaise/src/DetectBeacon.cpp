///
// DetectBeacon.cpp
//
#include <StandardCplusplus.h>
#include <phys253.h>
#include "pins.hpp"
#include "DetectBeacon.hpp"


const int BEACON_THRESHOLD {512};
const int *DetectBeacon::sensorPins {pins::BEACON_SENSORS_SIDES};


void DetectBeacon::init()
{
    MinorMode::init();
    
}


DetectBeacon::DetectBeacon()
    : MinorMode(), beaconThreshold(BEACON_THRESHOLD)
{
    this->init();
}


DetectBeacon::~DetectBeacon() {}


void DetectBeacon::loop() {}


// returns -1 for left, 1 for right, 0 for no detection
int DetectBeacon::getBeaconDirection()
{
    int val;
    int leftAnalog = analogRead(DetectBeacon::sensorPins[0]);
    int rightAnalog = analogRead(DetectBeacon::sensorPins[1]);
    
    if (leftAnalog > this->beaconThreshold) {
	if (rightAnalog > this->beaconThreshold)
	    return (leftAnalog > rightAnalog) ? -1 : 1;
	else
	    return -1;
    } else if (rightAnalog > this->beaconThreshold) 
	return 1;
    else
	return 0;
    
}

bool DetectBeacon::hasArrived()
{
    int leftAnalog = analogRead(DetectBeacon::sensorPins[0]);  // TODO: why cast this to int?
    int rightAnalog = analogRead(DetectBeacon::sensorPins[1]);
    return ((leftAnalog > this->beaconThreshold) ||
	    (rightAnalog > this->beaconThreshold));
}

// TODO
void DetectBeacon::test()
{
    MinorMode::test();
}
