///
// ToDestination.hpp
//
#ifndef TO_DESTINATION_HPP
#define TO_DESTINATION_HPP

#include "pins.hpp"
#include "Direction.hpp"

namespace ToDestination
{
    bool hasArrived();
    void loop();
    Direction getBeaconDirection();
}

//Function Declarations:::::::::::::
// void pickDirection();
// void updateLeftBeaconArray();
// void updateRightBeaconArray();
// int getLeftBeaconReading();
// int getRightBeaconReading();
// float getLeftBeaconAverage();
// float getRightBeaconAverage();
// void detectBeaconPrintLCD();

#endif // TO_DESTINATION_HPP
