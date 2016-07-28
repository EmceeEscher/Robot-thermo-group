///
// pins.hpp
//
// TINAH motor and sensor pin assignments
//
#ifndef PINS_HPP
#define PINS_HPP

#include <phys253pins.h>

namespace pins
{
    // Knobs
    const int KNOB_PROP_GAIN {6};
    const int KNOB_DER1_GAIN {7};
    
    // Digital in
    const int TAPE_SENSORS_FRONT[] { 0,  1,  2,  3};  // << < > >>
    const int TAPE_SENSORS_BACK[]  { 8,  9, 10, 11};  // << < > >>
    const int COLLISION_SENSORS[]  { 4, 5 };  // front, back
    const int ARM_SWITCHES[] {12, 13, 14}; //catch, miss, inner
    const int PASSENGER_SENSOR_BACK_LEFT {6}; //we ran out of analog pins, so this one has to be digital
    
    // Analog in
    const int PASSENGER_SENSORS_SIDES[] {0, 1, 2, 3, 4};
	  const int BEACON_SENSORS_SIDES[] {6,7}; // left right (Requires loss of knobs)
    const int POTENTIOMETER {5};

    // Digital out
    const int MOTOR_PIN_L {0};
    const int MOTOR_PIN_ARM {1};
    const int MOTOR_PIN_BABY {2};
    const int MOTOR_PIN_R {3};

    //Servo motors used as digital outputs for stepper motor
    const int PULSE_PIN {RCServo1Output};
    const int DIR_PIN {RCServo2Output};

    // Analog out

}

#endif  // PINS_HPP
