///
// MToDestination.cpp
//
#include "allminormodes.hpp"
#include "MToDestination.hpp"

int lastLeft;
int lastRight;
int currLeft;
int currRight;

void MToDestination::init()
{
  MajorMode::init();
  // TODO: reinitialize specific state variables
  lastLeft = mmDetectBeacon->getLeftReading();
  lastRight = mmDetectBeacon->getRightReading();
  currLeft = lastLeft;
  currRight = lastRight;
}

MToDestination::MToDestination(
    ArmControl      *mmArmControl,
    CollisionWatch  *mmCollisionWatch,
    DetectBeacon    *mmDetectBeacon,
    TapeFollow      *mmTapeFollow
    )
    : MajorMode(),
      mmArmControl(mmArmControl),
      mmCollisionWatch(mmCollisionWatch),
      mmDetectBeacon(mmDetectBeacon),
      mmTapeFollow(mmTapeFollow),
      state(MajModeEnum::DontChange){

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

    lastLeft = currLeft;
    lastRight = currRight;
    currLeft = mmDetectBeacon->getLeftReading();
    currRight = mmDetectBeacon->getRightReading();
    
    
    /*
     * 
     */
}

void MToDestination::start()
{
    MajorMode::start();

    this->mmTapeFollow->start();
    this->mmCollisionWatch->start();
    this->mmDetectBeacon->start();
    this->mmArmControl->start();
    this->state = MajModeEnum::DontChange;
}

void MToDestination::test()
{
    MajorMode::test();
    this->mmTapeFollow->test();
    this->mmCollisionWatch->test();
}
    
MajModeEnum MToDestination::changeTo(){
    return this->state;
}

