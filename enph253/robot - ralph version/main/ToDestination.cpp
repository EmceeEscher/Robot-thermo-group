#include <phys253.h>
#include <LiquidCrystal.h>
#include "ToDestination.hpp"
#include "TapeFollow.hpp"


//MToDestination Constants & Variables
const int DIFF_THRESHOLD = 50;

int lastLeft = 0;
int lastRight = 0;
int currLeft = 0;
int currRight = 0;
int lastDiff = 0;
int currDiff = 0;
//int printCount = 0;

//DetectBeacon Constants & Variables
const int BEACON_THRESHOLD = 100;
const int *beaconSensorPins(BEACON_SENSORS_SIDES);
const int numBeaconReadings = 5;

int leftBeaconReadings[numBeaconReadings];
float leftBeaconSum = 0;
int rightBeaconReadings[numBeaconReadings];
float rightBeaconSum = 0;
int beaconReadingIndex = 0;

void pickDirection(){
    
    float leftAverage = getLeftBeaconAverage();
    float rightAverage = getRightBeaconAverage();
    // if(printCount>25){
    // LCD.clear();
    // LCD.print("L: ");
    // LCD.print(leftAverage);
    // LCD.setCursor(0,1);
    // LCD.print("R: ");
    // LCD.print(rightAverage);
    // printCount = 0;
    // }
    // printCount++;
    
    float diff = rightAverage - leftAverage;
    if(abs(diff)>DIFF_THRESHOLD){
        if(diff>0){
            giveTurnDirection(0,100,0.1);
        }else{
            giveTurnDirection(100,0,0.1);
        }
    }else{
        if(diff>0){
            giveTurnDirection(0,0.1,100);
        }else{
            giveTurnDirection(0.1,0,100);
        }
    }
}

void detectBeaconLoop() {
    updateLeftBeaconArray();
    updateRightBeaconArray();
    beaconReadingIndex++;
    if(beaconReadingIndex >= numBeaconReadings){
        beaconReadingIndex = 0;    
    }
}

void updateLeftBeaconArray(){
    int newReading = getLeftBeaconReading();
    leftBeaconSum -= leftBeaconReadings[beaconReadingIndex];
    leftBeaconReadings[beaconReadingIndex] = newReading;
    leftBeaconSum += leftBeaconReadings[beaconReadingIndex];
}

void updateRightBeaconArray(){
    int newReading = getRightBeaconReading();
    rightBeaconSum -= rightBeaconReadings[beaconReadingIndex];
    rightBeaconReadings[beaconReadingIndex] = newReading;
    rightBeaconSum += rightBeaconReadings[beaconReadingIndex];
}

// returns -1 for left, 1 for right, 0 for no detection
Direction getBeaconDirection(){
    int val;
    float leftAverage = getLeftBeaconAverage();
    float rightAverage = getRightBeaconAverage();
    
    if (leftAverage > BEACON_THRESHOLD) {
        if (rightAverage > BEACON_THRESHOLD)
            return (leftAverage > rightAverage) ? Direction::LEFT : Direction::RIGHT;
        else
	    return Direction::LEFT;
    } else if (rightAverage > BEACON_THRESHOLD) 
        return Direction::RIGHT;
    else
        return Direction::FRONT;
    
}

int getLeftBeaconReading(){
    return analogRead(beaconSensorPins[0]);
}

int getRightBeaconReading(){
    return analogRead(beaconSensorPins[1]);
}

float getLeftBeaconAverage(){
    return leftBeaconSum / numBeaconReadings;
}

float getRightBeaconAverage(){
    return rightBeaconSum / numBeaconReadings;
}

bool hasArrived(){
    float leftAverage = getLeftBeaconAverage();
    float rightAverage = getRightBeaconAverage();
    return ((leftAverage > BEACON_THRESHOLD) ||
	    (rightAverage > BEACON_THRESHOLD));
}

void detectBeaconPrintLCD(){
  LCD.clear();
  LCD.print("LR: ");
  LCD.print(getLeftBeaconReading());
  LCD.print(" LA: ");
  LCD.print(getLeftBeaconAverage());
  LCD.setCursor(0,1);
  LCD.print("RR: ");
  LCD.print(getRightBeaconReading());
  LCD.print(" RA: ");
  LCD.print(getRightBeaconAverage());
}
