///
// TapeFollow2Disc.h
//
// Modified discrete threshold tape-following algorithm
//
#ifndef TAPE_FOLLOW_2_DISC_H
#define TAPE_FOLLOW_2_DISC_H

#pragma clang diagnostic push
#pragma ide diagnostic ignored "CannotResolve"

#include <phys253.h>

class TapeFollow2Disc {
public:
    TapeFollow2Disc(Tinah &t);
    void loop();
private:
    const double errord0F;  // error for 0th derivative change in position
    const double errord1F;  // error for 1st derivative change in position
    const double errord2F;  // error for 2nd derivative change in position
    const double gainPropl;
    const double gainDeriv;
    const double threshold;
    const int motorPinL;
    const int motorPinR;
    const int motorSpeed;
    const int reset;
    Tinah &tinah;
    int activePins[4];
    double readings[4];    // readings
    int tape[3][4];        // tape[0] : current read, tape[1] : previous, ...
    int timeMap[3];
    double errorPrev;
    double errorRecent;
    double errorNext;
    int timePrev;
    int timeNext;
    int count;
};

#pragma clang diagnostic pop

#endif // TAPE_FOLLOW_2_DISC_H
