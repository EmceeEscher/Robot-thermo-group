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
    : MajorMode(MajorModeEnum::DROP_PASSENGER, 3),  // 3 minor modes
      mmArmControl(mmArmControl),
      mmDetectBeacon(mmDetectBeacon),
      mmCollisionWatch(mmCollisionWatch)
{
    this->init();

    MinorMode **mm = new MinorMode*[3];
    mm[0] = this->mmArmControl;
    mm[1] = this->mmDetectBeacon;
    mm[2] = this->mmCollisionWatch;
    this->allMinorModes = mm;

    // // TODO: initialize specific minor modes
    // this->allMinorModes = {
    //         this->mmArmControl,
    //         this->mmDetectBeacon,
    //         this->mmCollisionWatch
    // };
}


MDropPassenger::~MDropPassenger() {}


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
