///
// DetectBeacon.hpp
//
// Minor mode for detecting drop-off beacon
//
#ifndef DETECT_BEACON_HPP
#define DETECT_BEACON_HPP

#include "MinorMode.hpp"

class DetectBeacon : public MinorMode
{

private:

    static const int *sensorPins;            // left right
    const int beaconThreshold;

    /*
     * (Re)initializes all state variables
     */
    void init();

public:

    DetectBeacon();   // constructor

    ~DetectBeacon();  // deconstructor

    /*
     * Main loop function to watch for collisions. Halts the TapeFollow
     * minor mode when a collision is detected
     */
    void loop();

    /*
     * Enter a testing mode in which sensor readings are still made, but
     * hardware (i.e. motors) are not active
     */
    void test();

    /*
     * Returns the number associated with the direction where the collision
     * occurred. [0, 1, 2, 3] = [left, front, right, back]
     * Returns -1 if no collision has been detected.
     */
    int getBeaconDirection();

    /*
     * Returns true if the robot has determined that it has arrived at the
     * drop-off location.
     */
     bool hasArrived();

};

#endif  // DETECT_BEACON_HPP
