///
// TapeFollow2Disc.h
//
// Modified discrete threshold tape-following algorithm
//
#ifndef TAPE_FOLLOW_2_DISC_H
#define TAPE_FOLLOW_2_DISC_H

#include <phys253.h>

class TapeFollow2Disc {
public:
    TapeFollow2Disc(Tinah &t);
    void loop();
private:
    const float errord0F;  // error for 0th derivative change in position
    const float errord1F;  // error for 1st derivative change in position
    const float errord2F;  // error for 2nd derivative change in position
    const float gainPropl;
    const float gainDeriv;
    const float threshold;
    const int motorPinL;
    const int motorPinR;
    const int motorSpeed;
    const int reset;
    Tinah tinah;
    int activePins[4];
    double readings[4];    // readings
    int tape[3][4];        // tape[0] : current read, tape[1] : previous, ...
    int timeMap[3];
    float errorPrev;
    float errorRecent;
    float errorNext;
    double ctrlPropl;
    double ctrlDeriv;
    double control;
    int timePrev;
    int timeNext;
    int count;
};

#endif // TAPE_FOLLOW_2_DISC_H
