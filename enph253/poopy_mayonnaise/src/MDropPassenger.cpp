///
// MDropPassenger.cpp
//
#include "allminormodes.hpp"
#include "MDropPassenger.hpp"


//TODO
void MDropPassenger::init()
{
    MajorMode::init();
    // TODO: reinitialize specific state variables
}

//TODO
MDropPassenger::MDropPassenger()
    : MajorMode()
{
  this->init();

  // TODO: initialize specific minor modes
  this->mmArmControl = new ArmControl;
  this->allMinorModes.push_back(mmArmControl);

  this->mmDetectBeacon = new DetectBeacon;
  this->allMinorModes.push_back(mmDetectBeacon);

  this->mmCollisionWatch = new CollisionWatch;
  this->allMinorModes.push_back(mmCollisionWatch);
}

MDropPassenger::~MDropPassenger(){}

// TODO

void MDropPassenger::loop()
{
    MajorMode::loop();

    if(this->mmDetectBeacon->hasArrived())
    {
        int dropoffDir = this->mmDetectBeacon->getBeaconDirection();
        if(dropoffDir == 1){
          this->mmArmControl->turnAndReach(true, false);
        }else if(dropoffDir == -1){
          this->mmArmControl->turnAndReach(false, false);
        }
    }
}

void MDropPassenger::start()
{
    MajorMode::start();

    this->mmArmControl->start();
    this->mmCollisionWatch->start();
    this->mmDetectBeacon->start();
}

// TODO
void MDropPassenger::test()
{
    MajorMode::test();

    this->mmArmControl->test();
    this->mmCollisionWatch->test();
    this->mmDetectBeacon->test();
}

