///
// TapeFollow.hpp
//
// Main tape following loop function
//
#ifndef TAPE_FOLLOW_HPP
#define TAPE_FOLLOW_HPP

//#pragma clang diagnostic push
//#pragma ide diagnostic ignored "CannotResolve"

#include <phys253.h>

class TapeFollow {
public:
    TapeFollow();
    void loop();
    void start();
    void stop();
private:
    const float smallError;     // error for when 1 sensor is over tape
    const float largeError;     // error if both sensors are off tape
    const int resetPeriod;      // number of steps to resent `count`
    const int propGainKnob;     // knob for proportional gain
    const int dervGainKnob;     // knob for derivative gain
    const int motorPinL;        // pin for left motor
    const int motorPinR;        // pin for right motor
    bool prevIntersections[2];  // L, R : previous intersection readings
    bool motorsActive;
    int motorSpeed;     // forward velocity of the robot
    int count;          // number of steps since last reset
    int turnDirection;  // direction robot is trying to turn
    int direction;      // move direction
    int prevTime;
    int timeStep;
    int error;
    int activePins[4];
    float lastError;
    float recentError;
};

//#pragma clang diagnostic pop

#endif // TAPE_FOLLOW_HPP