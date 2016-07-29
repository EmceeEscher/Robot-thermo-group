///
// DetectBeacon.cpp
//
#include <StandardCplusplus.h>
#include <string>
#include <phys253.h>
#include "pins.hpp"
#include "DetectBeacon.hpp"



void DetectBeacon::init()
{
    MinorMode::init();
    
}


DetectBeacon::DetectBeacon()
    : MinorMode(),
      sensorPins(pins::BEACON_SENSORS_SIDES)
{
    this->init();
}


DetectBeacon::~DetectBeacon() {}


void DetectBeacon::loop() {}

// returns -1 for left, 1 for right, 0 for no detection
int DetectBeacon::getBeaconDirection(){
	int val;
	int leftAnalog = analogRead(this->sensorPins[0]);
	int rightAnalog = analogRead(this->sensorPins[1]);
	
	if(leftAnalog > beaconThreshold){
		if(rightAnalog > beaconThreshold)
			return (leftAnalog > rightAnalog) ? -1 : 1;
		else
			return -1;
	}
	
	else if(rightAnalog > beaconThreshold){
		return 1;
	}
	
	else
		return 0;
	
}

// TODO
void DetectBeacon::test()
{
    MinorMode::test();
}