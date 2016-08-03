///
// ToDestination.cpp
//
#include <phys253.h>
#include <LiquidCrystal.h>
#include "ToDestination.hpp"
#include "TapeFollow.hpp"


//MToDestination Constants & Variables
const int DIFF_THRESHOLD = 50;

//DetectBeacon Constants & Variables
const int BEACON_THRESHOLD = 50;
const int ARRIVED_THRESHOLD = 650;
const int *beaconSensorPins {pins::BEACON_SENSORS_SIDES};
const int numBeaconReadings = 5;

static int leftBeaconReadings[numBeaconReadings];
static float leftBeaconSum = 0;
static int rightBeaconReadings[numBeaconReadings];
static float rightBeaconSum = 0;
static int beaconReadingIndex = 0;


namespace ToDestination
{
    // static void pickDirection();
    static void updateLeftBeaconArray();
    static void updateRightBeaconArray();
    static int getLeftBeaconReading();
    static int getRightBeaconReading();
    static float getLeftBeaconAverage();
    static float getRightBeaconAverage();
    static void pickDirection();
    // static void detectBeaconPrintLCD();
}


void ToDestination::init()
{
    for (int i(0); i < numBeaconReadings; ++i) {
        leftBeaconReadings[i] = getLeftBeaconReading();
        rightBeaconReadings[i] = getRightBeaconReading();
    }
}


void ToDestination::pickDirection()
{
    float leftAverage = getLeftBeaconAverage();
    float rightAverage = getRightBeaconAverage();
    float diff = rightAverage - leftAverage;
    if (abs(diff) > DIFF_THRESHOLD) {
        if (diff > 0) 
            TapeFollow::giveTurnDirection(0, 100, 0.1);
        else
            TapeFollow::giveTurnDirection(100,0,0.1);
    } else {
        if (diff > 0) 
            TapeFollow::giveTurnDirection(0, 0.1, 100);
        else
            TapeFollow::giveTurnDirection(0.1, 0, 100);
    }
}


void ToDestination::loop()
{
    updateLeftBeaconArray();
    updateRightBeaconArray();
    ++beaconReadingIndex;
    if (beaconReadingIndex >= numBeaconReadings) 
        beaconReadingIndex = 0;    
}


void ToDestination::updateLeftBeaconArray()
{
    int newReading = getLeftBeaconReading();
    leftBeaconSum -= leftBeaconReadings[beaconReadingIndex];
    leftBeaconReadings[beaconReadingIndex] = newReading;
    leftBeaconSum += leftBeaconReadings[beaconReadingIndex];
}


void ToDestination::updateRightBeaconArray()
{
    int newReading = getRightBeaconReading();
    rightBeaconSum -= rightBeaconReadings[beaconReadingIndex];
    rightBeaconReadings[beaconReadingIndex] = newReading;
    rightBeaconSum += rightBeaconReadings[beaconReadingIndex];
}


// returns -1 for left, 1 for right, 0 for no detection
Direction ToDestination::getBeaconDirection()
{
    float leftAverage = getLeftBeaconAverage();
    float rightAverage = getRightBeaconAverage();
    
    if (leftAverage > BEACON_THRESHOLD) {
        if (rightAverage > BEACON_THRESHOLD)
            return (leftAverage > rightAverage) ?
                Direction::LEFT : Direction::RIGHT;
        else
	    return Direction::LEFT;
    } else if (rightAverage > BEACON_THRESHOLD) 
        return Direction::RIGHT;
    else
        return Direction::FRONT;
    
}


int ToDestination::getLeftBeaconReading()
{
    return analogRead(beaconSensorPins[0]);
}


int ToDestination::getRightBeaconReading()
{
    return analogRead(beaconSensorPins[1]);
}


float ToDestination::getLeftBeaconAverage()
{
    return leftBeaconSum / numBeaconReadings;
}


float ToDestination::getRightBeaconAverage()
{
    return rightBeaconSum / numBeaconReadings;
}


bool ToDestination::hasArrived()
{
    float leftAverage = getLeftBeaconAverage();
    float rightAverage = getRightBeaconAverage();
    return ((leftAverage > ARRIVED_THRESHOLD) ||
	    (rightAverage > ARRIVED_THRESHOLD));
}


void ToDestination::printLCD()
{
    LCD.clear();
    LCD.print(F("LR: "));
    LCD.print(getLeftBeaconReading());
    LCD.print(F(" LA: "));
    LCD.print(getLeftBeaconAverage());
    LCD.setCursor(0,1);
    LCD.print(F("RR: "));
    LCD.print(getRightBeaconReading());
    LCD.print(F(" RA: "));
    LCD.print(getRightBeaconAverage());
}

