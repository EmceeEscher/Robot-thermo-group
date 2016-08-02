
#ifndef PINS_HPP
#define PINS_HPP

#include <phys253pins.h>


// Knobs
const int KNOB_PROP_GAIN {6};
const int KNOB_DER1_GAIN {7};

// Digital in
const int TAPE_SENSORS_FRONT[] { 0,  1,  2,  3};  // << < > >>
const int TAPE_SENSORS_BACK[]  { 8,  9, 10, 11};  // << < > >>
const int ARM_SWITCHES[]       {12, 13, 14};      //catch, miss, inner
const int COLLISION_SENSORS[]  { 4,  5};  // left front right back

// Analog in
const int PASSENGER_SENSORS_SIDES[]  {2, 7, 6, 0}; //side-left, front-left, front-right, side-right
const int BEACON_SENSORS_SIDES[]     {3, 4}; // left right (Requires loss of knobs)
const int POTENTIOMETER {5};

// Digital out
const int MOTOR_PIN_L    {0};
const int MOTOR_PIN_ARM  {1};
const int MOTOR_PIN_BABY {2};
const int MOTOR_PIN_R    {3};

//Servo motors used as digital outputs for stepper motor
const int PULSE_PIN {RCServo1Output};
const int DIR_PIN   {RCServo2Output};

// Analog out


/*
  namespace pins_sizes
  {
  // Digital in
  const int TAPE_SENSORS_FRONT = sizeof(pins::TAPE_SENSORS_FRONT) /
  sizeof(pins::TAPE_SENSORS_FRONT[0]);
  const int TAPE_SENSORS_BACK = sizeof(pins::TAPE_SENSORS_BACK) /
  sizeof(pins::TAPE_SENSORS_BACK[0]);
  const int ARM_SWITCHES = sizeof(pins::ARM_SWITCHES) /
  sizeof(pins::ARM_SWITCHES[0]);
  const int COLLISION_SENSORS = sizeof(pins::COLLISION_SENSORS) /
  sizeof(pins::COLLISION_SENSORS[0]);
  
  // Analog in
  const int PASSENGER_SENSORS_SIDES = sizeof(pins::PASSENGER_SENSORS_SIDES) /
  sizeof(pins::PASSENGER_SENSORS_SIDES[0]);
  const int BEACON_SENSORS_SIDES = sizeof(pins::BEACON_SENSORS_SIDES) /
  sizeof(pins::BEACON_SENSORS_SIDES[0]);
  }
*/

#endif
