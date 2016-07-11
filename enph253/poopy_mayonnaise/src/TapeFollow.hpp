///
// TapeFollow.h
//
// Main tape following loop function
//
#ifndef TAPE_FOLLOW_H
#define TAPE_FOLLOW_H

#pragma clang diagnostic push
#pragma ide diagnostic ignored "CannotResolve"

#include <phys253.h>

class TapeFollow {
 public:
    TapeFollow(Tinah &t);
    void loop();
 private:
    const float smallError;
    const float largeError;
    const int motorSpeed;
    const int resetPeriod;
    const int threshold;
    const int propGainKnob;
    const int dervGainKnob;
    const int motorPinL;
    const int motorPinR;
    const int overTape;
    const int offTape;
    Tinah &tinah;
    int count;
    int turnDirection;
    int direction;
    int prevTime;
    int timeStep;
    int error;
    float lastError;
    float recentError;
    int intersections[2];
    int activePins[4];
    double pinReadings[4];
};

#pragma clang diagnostic pop

#endif // TAPE_FOLLOW_H
