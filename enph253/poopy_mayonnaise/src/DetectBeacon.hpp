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

    /*
     * updates the array used for averaging the readings of the left sensor
     */
    void updateLeftArray();

    /*
     * updates the array used for averaging the readings of the right sensor
     */
    void updateRightArray();

public:

    DetectBeacon();   // constructor

    ~DetectBeacon();  // deconstructor


    void loop();

    void test();

    /*
     * Tells whether beacon is closer to right side or to left side.
     * Returns -1 if left, 1 if right, 0 if can't see beacon
     */
    int getBeaconDirection();

    /*
     * Returns analog reading of left beacon detector.
     */
    int getLeftReading();

    /*
     * Returns analog reading of right beacon detector.
     */
    int getRightReading();

        /*
     * Returns average of last numReadings readings of left beacon detector.
     */
    float getLeftAverage();

    /*
     * Returns average of last numReadings readings of right beacon detector.
     */
    float getRightAverage();


    /*
     * Returns true if the robot has determined that it has arrived at the
     * drop-off location.
     */
     bool hasArrived();

};

#endif  // DETECT_BEACON_HPP
