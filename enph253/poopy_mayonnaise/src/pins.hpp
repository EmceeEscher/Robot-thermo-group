///
// pins.hpp
//
// TINAH motor and sensor pin assignments
//
#ifndef PINS_HPP
#define PINS_HPP

#include <StandardCplusplus.h>
#include <vector>
#include <map>
#include <utility.h>
// #include "Direction.hpp"

namespace pins
{
    using std::vector;
    using std::map;

    // Knobs
    const int KNOB_PROP_GAIN {6};
    const int KNOB_DER1_GAIN {7};
    
    // Digital in
    const vector<int> TAPE_SENSORS_FRONT { 0,  1,  2,  3};
    const vector<int> TAPE_SENSORS_BACK  { 8,  9, 10, 11};
    const vector<int> COLLISION_SENSORS  { 4,  5,  6   7};
    // const vector< std::pair<Direction, int> > COLLISION_SENSORS_MAP {  // <- Stupid subsstitute for a constant map
    // 	    std::pair<Direction, int>(Direction::LEFT,   8),
    //         std::pair<Direction, int>(Direction::FRONT,  9),
    //         std::pair<Direction, int>(Direction::RIGHT, 10),
    //         std::pair<Direction, int>(Direction::BACK,  11)
    // };
    
    // Analog in
    const vector<int> PASSENGER_SENSORS_SIDES {0, 1, 2, 3, 4, 5};

    // Digital out
    const int MOTOR_PIN_L {0};
    const int MOTOR_PIN_R {3};

    // Analog out

}

#endif  // PINS_HPP
