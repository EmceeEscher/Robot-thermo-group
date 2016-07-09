///
// TapeFollow.h
//
// Main tape following loop function
//
#ifndef TAPE_FOLLOW_H
#define TAPE_FOLLOW_H

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
    Tinah tinah;
    int count;
    int turnDirection;
    int direction;
    int prevTime;
    int timeStep;
    int error;
    int lastError;
    int recentError;
    int intersections[2];
    int activePins[4];
    double pinReadings[4];
};

#endif // TAPE_FOLLOW_H
