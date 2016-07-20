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

    const vector<int> qsdPinsSides;                 // left-back, left-mid, left-front, right-front, right-mid, right-back

    bool approachingPassenger;
    bool atPassenger;

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
    
};

#endif  // PASSENGER_SEEK_HPP
