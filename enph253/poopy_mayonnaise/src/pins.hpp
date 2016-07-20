///
// pins.hpp
//
// TINAH motor and sensor pin assignments
//
#ifndef PINS_HPP
#define PINS_HPP

#include <StandardCplusplus.h>
#include <vector>

namespace pins
{
    using std::vector;

    // Knobs
    const int KNOB_PROP_GAIN {6};
    const int KNOB_DER1_GAIN {7};
    
    // Digital in
    const vector<int> TAPE_SENSORS_FRONT {0, 1, 2, 3};
    const vector<int> TAPE_SENSORS_BACK  {4, 5, 6, 7};
    
    // Analog in
    const vector<int> PASSENGER_SENSORS_SIDES {0, 1, 2, 3, 4, 5};

    // Digital out
    const int MOTOR_PIN_L {0};
    const int MOTOR_PIN_R {3};

    // Analog out

}

#endif  // PINS_HPP
