#include <phys253.h>
#include <LiquidCrystal.h>
#include "FindBeacon.hpp"
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
const int BEACON_THRESHOLD = 10;
const int ARRIVED_THRESHOLD = 500;
const int *beaconSensorPins(BEACON_SENSORS_SIDES);
const int numBeaconReadings = 5;

int leftBeaconReadings[numBeaconReadings];
float leftBeaconSum = 0;
int rightBeaconReadings[numBeaconReadings];
float rightBeaconSum = 0;
int beaconReadingIndex = 0;

void FindBeacon::init(){
  int i;
  for(i = 0; i < numBeaconReadings; i++){
    //leftBeaconReadings[i] = getLeftBeaconReading();
    leftBeaconReadings[i] = 0;
    leftBeaconSum = 0;
    //rightBeaconReadings[i] = getRightBeaconReading();
    rightBeaconReadings[i] = 0;
    rightBeaconSum = 0;
  }
}

void FindBeacon::pickDirection(){
    
    float left = getLeftBeaconReading();
    float right = getRightBeaconReading();

    //the following was changed mid-competition due to problems with the arm swinging
    /*float diff = right - left;
    if(abs(diff)>DIFF_THRESHOLD){
        if(diff>0){
            giveTurnDirection(0,100,0.1);
        }else{
            giveTurnDirection(100,0,0.1);
        }
    }else{
        if(diff>0){
            giveTurnDirection(10,25,100);
        }else{
            giveTurnDirection(25,10,100);
        }
    }*/
    TapeFollow::giveTurnDirection(50,50,50);
}

void FindBeacon::loop() {
    FindBeacon::updateLeftBeaconArray();
    FindBeacon::updateRightBeaconArray();
    beaconReadingIndex++;
    if(beaconReadingIndex >= numBeaconReadings){
        beaconReadingIndex = 0;    
    }
}

void FindBeacon::updateLeftBeaconArray(){
    int newReading = FindBeacon::getLeftBeaconReading();
    leftBeaconSum -= leftBeaconReadings[beaconReadingIndex];
    leftBeaconReadings[beaconReadingIndex] = newReading;
    leftBeaconSum += leftBeaconReadings[beaconReadingIndex];
}

void FindBeacon::updateRightBeaconArray(){
    int newReading = FindBeacon::getRightBeaconReading();
    rightBeaconSum -= rightBeaconReadings[beaconReadingIndex];
    rightBeaconReadings[beaconReadingIndex] = newReading;
    rightBeaconSum += rightBeaconReadings[beaconReadingIndex];
}

// returns -1 for left, 1 for right, 0 for no detection
Direction FindBeacon::getBeaconDirection(){
    int val;
    float leftAverage = FindBeacon::getLeftBeaconAverage();
    float rightAverage = FindBeacon::getRightBeaconAverage();
    
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

int FindBeacon::getLeftBeaconReading(){
    return analogRead(beaconSensorPins[0]);
}

int FindBeacon::getRightBeaconReading(){
    return analogRead(beaconSensorPins[1]);
}

float FindBeacon::getLeftBeaconAverage(){
    return leftBeaconSum / numBeaconReadings;
}

float FindBeacon::getRightBeaconAverage(){
    return rightBeaconSum / numBeaconReadings;
}

bool FindBeacon::hasArrived(){
    float leftAverage = FindBeacon::getLeftBeaconAverage();
    float rightAverage = FindBeacon::getRightBeaconAverage();
    return ((leftAverage > ARRIVED_THRESHOLD) ||
	    (rightAverage > ARRIVED_THRESHOLD));
}

void FindBeacon::printLCD(){
  LCD.clear();
  LCD.print("LR: ");
  LCD.print(FindBeacon::getLeftBeaconReading());
  LCD.print(" LA: ");
  LCD.print(FindBeacon::getLeftBeaconAverage());
  LCD.setCursor(0,1);
  LCD.print("RR: ");
  LCD.print(FindBeacon::getRightBeaconReading());
  LCD.print(" RA: ");
  LCD.print(FindBeacon::getRightBeaconAverage());
}

