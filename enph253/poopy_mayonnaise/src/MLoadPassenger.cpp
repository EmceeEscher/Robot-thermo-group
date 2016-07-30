///
// MLoadPassenger.cpp
//
#include <phys253.h>
#include "allminormodes.hpp"
#include "MajorModeEnum.hpp"
#include "MLoadPassenger.hpp"


void MLoadPassenger::init()
{
    MajorMode::init();
    //TODO: reinitialize specific state variables
}


// TODO
MLoadPassenger::MLoadPassenger(
        ArmControl      *mmArmControl,
        PassengerSeek   *mmPassengerSeek,
        CollisionWatch  *mmCollisionWatch)
    : MajorMode(MajorModeEnum::LOAD_PASSENGER, 3),  // 3 minor modes
      mmArmControl(mmArmControl),
      mmPassengerSeek(mmPassengerSeek),
      mmCollisionWatch(mmCollisionWatch)
{
    this->init();
    
    MinorMode **mm = new MinorMode*[3];
    mm[0] = this->mmArmControl;
    mm[1] = this->mmCollisionWatch;
    mm[2] = this->mmPassengerSeek;
    
    // // TODO: initialize specific minor modes
    // this->allMinorModes = {
    // 	    this->mmArmControl,
    // 	    this->mmCollisionWatch,
    // 	    this->mmPassengerSeek
    // };
}


MLoadPassenger::~MLoadPassenger() {}


//TODO
void MLoadPassenger::loop()
{
    if (this->mmCollisionWatch->collisionHasOccurred()) {
	//TODO: figure out what to do other than panic
	//UPDATE: just f*cking ignore it
    }
    
    if (this->mmPassengerSeek->isAtPassenger()) {
	LCD.clear();
	if (this->mmPassengerSeek->getPassengerSide() == 1) {
	    LCD.print("reaching on right");
	    this->mmArmControl->turnAndReach(true, true);
	} else if (this->mmPassengerSeek->getPassengerSide() == -1) {
	    LCD.print("reaching on left");
	    this->mmArmControl->turnAndReach(false, true);
	}
	if (this->mmArmControl->isHolding()) 
	    this->nextMode = MajorModeEnum::TO_DESTINATION;
	else {
	    //reposition
	}
    } else {
	//reposition
    }
}


void MLoadPassenger::start()
{
    MajorMode::start();

    //start minor modes
    this->mmArmControl->start();
    this->mmCollisionWatch->start();
    this->mmPassengerSeek->start();
}


//TODO
void MLoadPassenger::test()
{
    MajorMode::test();
}
