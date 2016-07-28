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

    // Digital out
    const int MOTOR_PIN_L {0};
    const int MOTOR_PIN_R {3};

    // Analog out

}

namespace pins_sizes
{
    // Digital in
    const int TAPE_SENSORS_FRONT = sizeof(pins::TAPE_SENSORS_FRONT) /
            sizeof(pins::TAPE_SENSORS_FRONT[0]);
    const int TAPE_SENSORS_BACK = sizeof(pins::TAPE_SENSORS_BACK) /
	    sizeof(pins::TAPE_SENSORS_BACK[0]);
    const int COLLISION_SENSORS = sizeof(pins::COLLISION_SENSORS) /
	    sizeof(pins::COLLISION_SENSORS[0]);

    // Analog in
    const int PASSENGER_SENSORS_SIDES = sizeof(pins::PASSENGER_SENSORS_SIDES) /
	    sizeof(pins::PASSENGER_SENSORS_SIDES[0]);
}

#endif  // PINS_HPP
