///
// MToDestination.cpp
//
#include "allminormodes.hpp"
#include "MToDestination.hpp"
#include <phys253.h>

const int DIFF_THRESHOLD = 50;

int lastLeft;
int lastRight;
int currLeft;
int currRight;
int lastDiff;
int currDiff;
int printCount;

void MToDestination::init()
{
  MajorMode::init();
  // TODO: reinitialize specific state variables
  lastLeft = mmDetectBeacon->getLeftReading();
  lastRight = mmDetectBeacon->getRightReading();
  currLeft = lastLeft;
  currRight = lastRight;
  lastDiff = currRight - currLeft;
  currDiff = lastDiff;
  printCount = 0;
}

MToDestination::MToDestination(
    ArmControl      *mmArmControl,
    CollisionWatch  *mmCollisionWatch,
    DetectBeacon    *mmDetectBeacon,
    TapeFollow      *mmTapeFollow
    )
    : MajorMode(MajorModeEnum::TO_DESTINATION),
      mmArmControl(mmArmControl),
      mmCollisionWatch(mmCollisionWatch),
      mmDetectBeacon(mmDetectBeacon),
      mmTapeFollow(mmTapeFollow),
      diffThreshold(DIFF_THRESHOLD){

        this->init();

        this->allMinorModes.push_back(mmArmControl);
        this->allMinorModes.push_back(mmTapeFollow);
        this->allMinorModes.push_back(mmCollisionWatch);
        this->allMinorModes.push_back(mmDetectBeacon);
      }

MToDestination::~MToDestination() {}

void MToDestination::loop()
{
    MajorMode::loop();

    if(this->mmCollisionWatch->collisionHasOccurred()){
      this->mmTapeFollow->turnAround();
    }

    this->pickDirection();
    
    if(this->mmDetectBeacon->hasArrived()){
      this->nextMode = MajorModeEnum::DROP_PASSENGER;
    }
    /*
     * tapeFollow
     * if at intersection, turn whichever direction has higher reading
     *    if you can't turn that direction, turn whichever is closest
     * if reading passes threshold, drop off
     */
}

void MToDestination::pickDirection(){
  
    float leftAverage = mmDetectBeacon->getLeftAverage();
    float rightAverage = mmDetectBeacon->getRightAverage();
    if(printCount>25){
      LCD.clear();
      LCD.print("L: ");
      LCD.print(leftAverage);
      LCD.setCursor(0,1);
      LCD.print("R: ");
      LCD.print(rightAverage);
      printCount = 0;
    }
    printCount++;

    float diff = rightAverage - leftAverage;
    if(abs(diff)>this->diffThreshold){
      if(diff>0){
        this->mmTapeFollow->giveTurnDirection(0,100,0.1);
      }else{
        this->mmTapeFollow->giveTurnDirection(100,0,0.1);
      }
    }else{
      if(diff>0){
        this->mmTapeFollow->giveTurnDirection(0,0.1,100);
      }else{
        this->mmTapeFollow->giveTurnDirection(0.1,0,100);
      }
    }
}

void MToDestination::start()
{
    MajorMode::start();

    this->mmTapeFollow->start();
    this->mmCollisionWatch->start();
    this->mmDetectBeacon->start();
    this->mmArmControl->start();
}

void MToDestination::test()
{
    MajorMode::test();
    this->mmTapeFollow->test();
    this->mmCollisionWatch->test();
}

