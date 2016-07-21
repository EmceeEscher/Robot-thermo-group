///
// PassengerSeek.hpp
//
// Minor mode for seeking passenger signals
//
#ifndef PASSENGER_SEEK_HPP
#define PASSENGER_SEEK_HPP

#include <StandardCplusplus.h>
#include <vector>
#include "MinorMode.hpp"

using std::vector;

class PassengerSeek : public MinorMode
{

private:

    const int maxRegisterPeriod;                    // number of consecutive (+) derivatives to be increasing and (-) derivative to be decreasing
    const double maxRegisterThreshold;              // threshold that readings must be above to register
    const vector<int> qsdPinsSides;                 // left-back, left-mid, left-front, right-front, right-mid, right-back

    bool approachingPassenger;                      // true when approaching a passenger
    bool atPassenger;                               // true when adjacent to a passenger
    int passengerSide;                              // if atPassenger, specifies the side (-1=left, 1=right)

    vector<bool> atMax;                             // true if the associated pin is at a maximum
    vector<double> pinReadings;                     // current pin readings
    vector< vector<double> > lastPinReadings;       // vector of 6-vectors containing historical pin reading data
    vector< vector<double> > lastPinReadingsDeriv;  // vector of 6-vectors containing derivatives of lastPinReadings

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
