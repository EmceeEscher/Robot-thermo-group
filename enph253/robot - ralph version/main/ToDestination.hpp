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
    void printLCD();
    void init();
}

#endif // TO_DESTINATION_HPP
