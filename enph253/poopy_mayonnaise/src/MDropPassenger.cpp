///
// MDropPassenger.cpp
//
#include "allminormodes.hpp"
#include "MajorModeEnum.hpp"
#include "MDropPassenger.hpp"


//TODO
void MDropPassenger::init()
{
    MajorMode::init();
    // TODO: reinitialize specific state variables
}


//TODO
MDropPassenger::MDropPassenger(
        ArmControl      *mmArmControl,
	DetectBeacon    *mmDetectBeacon,
	CollisionWatch  *mmCollisionWatch)
    : MajorMode(MajorModeEnum::DROP_PASSENGER),
      mmArmControl(mmArmControl),
      mmDetectBeacon(mmDetectBeacon),
      mmCollisionWatch(mmCollisionWatch)
{
  this->init();

  // TODO: initialize specific minor modes
  this->allMinorModes.push_back(mmArmControl);
  this->allMinorModes.push_back(mmDetectBeacon);
  this->allMinorModes.push_back(mmCollisionWatch);
}


MDropPassenger::~MDropPassenger(){}


// TODO
void MDropPassenger::loop()
{
    MajorMode::loop();

    if (this->mmDetectBeacon->hasArrived()) {
        int dropoffDir = this->mmDetectBeacon->getBeaconDirection();
        if (dropoffDir == 1) 
	    this->mmArmControl->turnAndReach(true, false);
	else if (dropoffDir == -1) 
	    this->mmArmControl->turnAndReach(false, false);
    }
    if (!this->mmArmControl->isHolding())
	this->nextMode = MajorModeEnum::FIND_PASSENGER;
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
