///
// pins.hpp
//
// TINAH motor and sensor pin assignments
//
#ifndef PINS_HPP
#define PINS_HPP

namespace pins
{
    // Knobs
    const int KNOB_PROP_GAIN {6};
    const int KNOB_DER1_GAIN {7};
    
    // Digital in
    const int TAPE_SENSORS_FRONT[] { 0,  1,  2,  3};  // << < > >>
    const int TAPE_SENSORS_BACK[]  { 8,  9, 10, 11};  // << < > >>
    const int COLLISION_SENSORS[]  { 5,  4,  6,  7};  // left front right back
    
    // Analog in
    const int PASSENGER_SENSORS_SIDES[] {0, 1, 2, 3, 4, 5};
	const int BEACON_SENSORS_SIDES[] {6,7}; // left right (Requires loss of knobs)

    // Digital out
    const int MOTOR_PIN_L {0};
    const int MOTOR_PIN_R {3};

    // Analog out

}

#endif  // PINS_HPP
