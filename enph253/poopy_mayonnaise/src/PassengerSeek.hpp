///
// PassengerSeek.hpp
//
// Minor mode for seeking passenger signals
//
#ifndef PASSENGER_SEEK_HPP
#define PASSENGER_SEEK_HPP

#include <StandardCplusplus.h>
#include <bitset>
#include "pins.hpp"
#include "MinorMode.hpp"

using std::bitset;

class PassengerSeek : public MinorMode
{

private:

    static const int *qsdPinsSides;     // left-back, left-mid, left-front, right-front, right-mid, right-back
    static const int numPinsSides = pins_sizes::PASSENGER_SENSORS_SIDES;

    const int maxRegisterPeriod;        // number of consecutive (+) derivatives to be increasing and (-) derivative to be decreasing
    const float maxRegisterThreshold;   // threshold that readings must be above to register
    
    bool approachingPassenger;          // true when approaching a passenger
    bool atPassenger;                   // true when adjacent to a passenger
    int passengerSide;                  // if atPassenger, specifies the side (-1=left, 1=right)

    bitset<numPinsSides> atMax;                    // true if the associated pin is at a maximum
    int pinReadings[numPinsSides];               // current pin readings
    int lastPinReadings[numPinsSides];           // pin readings from last loop

    int numAboveThreshold[numPinsSides];           // number of consecutive reads above threshold for each pin
    int numPosDeriv[numPinsSides];                 // number of consecutive positive derivatives THE LAST TIME A POSITIVE DERIVATIVE WAS READ
    int numNegDeriv[numPinsSides];                 // number of consecutive negative or 0 derivatives THE LAST TIME A NEGATIVE DERIVATIVE WAS READ
    bitset<numPinsSides> lastDerivPositive;        // true if the last derivative was negative

    /*
     * (Re)initialize all state variables
     */
    void init();

    /*
     * Return true if one of the front side readings is at a maximum
     */
    bool atMaxSideFront();

    /*
     * Return true if one of the middle side readings is at a maximum
     */
    bool atMaxSideMiddle();

    /*
     * Update the atMax array to reflect the latest readings
     */
    void updateMax();

public:
    
    PassengerSeek();   // constructor

    ~PassengerSeek();  // deconstructor

    /*
     * Main loop function to seek passengers. Halts the TapeFollow minor
     * mode when the passenger is directly adjacent to the robot
     */
    void loop();

    /*
     * Enter a mode where readings are still made, but hardware (i.e. motors)
     * are not active
     */
    void test();

    /*
     * Returns true if one of the front side pins is at a maximum
     */
    bool isApproachingPassenger();

    /*
     * Returns true if the middle (arm) pin is at a maximum after, the
     * front maximum was seen
     */
    bool isAtPassenger();

    /*
     * If at passenger, returns 1 if the passenger is on the right and -1
     * if the passenger is on the left.
     * Behavior is undefined if we are not at a passenger.
     */
    int getPassengerSide();
    
};

#endif  // PASSENGER_SEEK_HPP
