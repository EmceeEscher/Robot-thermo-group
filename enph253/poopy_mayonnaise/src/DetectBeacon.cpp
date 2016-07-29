///
// DetectBeacon.cpp
//
#include <StandardCplusplus.h>
#include <string>
#include <phys253.h>
#include "pins.hpp"
#include "DetectBeacon.hpp"


const int BEACON_THRESHOLD = 512;
const int *DetectBeacon::sensorPins(pins::BEACON_SENSORS_SIDES);
int numReadings = 5;
int* leftReadings;
float leftSum;
int* rightReadings;
float rightSum;
int readingIndex = 0;

void DetectBeacon::init()
{
    MinorMode::init();
    int leftReading = DetectBeacon::getLeftReading();
    int rightReading = DetectBeacon::getRightReading();
    leftSum = 0;
    rightSum = 0;
    leftReadings = new int[numReadings];
    rightReadings = new int[numReadings];
    for(readingIndex = 0; readingIndex < numReadings; readingIndex++){
      leftReadings[readingIndex] = leftReading;
      rightReadings[readingIndex] = rightReading;
      leftSum += leftReading;
      rightSum += rightReading;
    }
    readingIndex = 0;
}


DetectBeacon::DetectBeacon()
    : MinorMode(),
      beaconThreshold(BEACON_THRESHOLD)
{
    this->init();
}


DetectBeacon::~DetectBeacon() {}


void DetectBeacon::loop() {
  DetectBeacon::updateLeftArray();
  DetectBeacon::updateRightArray();
  readingIndex++;
  if(readingIndex >= numReadings){
    readingIndex = 0;    
  }
}

void DetectBeacon::updateLeftArray(){
  int newReading = DetectBeacon::getLeftReading();
  leftSum -= leftReadings[readingIndex];
  leftReadings[readingIndex] = newReading;
  leftSum += leftReadings[readingIndex];
}

void DetectBeacon::updateRightArray(){
  int newReading = DetectBeacon::getRightReading();
  rightSum -= rightReadings[readingIndex];
  rightReadings[readingIndex] = newReading;
  rightSum += rightReadings[readingIndex];
}

// returns -1 for left, 1 for right, 0 for no detection
int DetectBeacon::getBeaconDirection(){
    int val;
    float leftAverage = DetectBeacon::getLeftAverage();
    float rightAverage = DetectBeacon::getRightAverage();
    
    if (leftAverage > this->beaconThreshold) {
	    if (rightAverage > this->beaconThreshold)
	      return (leftAverage > rightAverage) ? -1 : 1;
	  else
	    return -1;
    } else if (rightAverage > this->beaconThreshold) 
	      return 1;
    else
	      return 0;
    
}

int DetectBeacon::getLeftReading(){
  return analogRead(this->sensorPins[0]);
}

int DetectBeacon::getRightReading(){
  return analogRead(this->sensorPins[1]);
}

float DetectBeacon::getLeftAverage(){
  return leftSum / numReadings;
}

float DetectBeacon::getRightAverage(){
  return rightSum / numReadings;
}

bool DetectBeacon::hasArrived(){
    float leftAverage = DetectBeacon::getLeftAverage();
    float rightAverage = DetectBeacon::getRightAverage();
    return ((leftAverage > this->beaconThreshold) ||
	    (rightAverage > this->beaconThreshold));
}

// TODO
void DetectBeacon::test()
{
    MinorMode::test();
}
