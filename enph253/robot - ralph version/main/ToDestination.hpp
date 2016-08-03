#ifndef TO_DESTINATION_HPP
#define TO_DESTINATION_HPP

#include "pins.hpp"
#include "Direction.hpp"

//Function Declarations:::::::::::::
void beaconInit();
void pickDirection();
void detectBeaconLoop();
void updateLeftBeaconArray();
void updateRightBeaconArray();
Direction getBeaconDirection();
int getLeftBeaconReading();
int getRightBeaconReading();
float getLeftBeaconAverage();
float getRightBeaconAverage();
bool hasArrived();
void detectBeaconPrintLCD();

#endif // TO_DESTINATION_HPP
