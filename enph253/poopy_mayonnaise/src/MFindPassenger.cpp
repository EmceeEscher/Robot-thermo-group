///
// MFindPassenger.cpp
//
#include <phys253.h>
#include "allminormodes.hpp"
#include "MajorModeEnum.hpp"
#include "MFindPassenger.hpp"


// TODO
void MFindPassenger::init()
{
    MajorMode::init();
    // TODO: reinitialize specific state variables
}


// TODO
MFindPassenger::MFindPassenger(
        ArmControl     *mmArmControl,
        TapeFollow     *mmTapeFollow,
	PassengerSeek  *mmPassengerSeek,
	CollisionWatch *mmCollisionWatch
)
    : MajorMode(MajorModeEnum::FIND_PASSENGER),
      mmArmControl     (mmArmControl),
      mmTapeFollow     (mmTapeFollow),
      mmPassengerSeek  (mmPassengerSeek),
      mmCollisionWatch (mmCollisionWatch)
{
    this->init();

    // TODO: initialize specific minor modes
    this->allMinorModes.push_back(mmArmControl);
    this->allMinorModes.push_back(mmTapeFollow);
    this->allMinorModes.push_back(mmCollisionWatch);
    this->allMinorModes.push_back(mmPassengerSeek);
}


MFindPassenger::~MFindPassenger() {}


// TODO
void MFindPassenger::loop()
{
    MajorMode::loop();  // does loop for each active minor mode

    if (this->mmCollisionWatch->collisionHasOccurred()) {
    	// for now, just turn around
	this->mmTapeFollow->turnAround();
    }

    // only seek passengers when not turning or seeking
    // if (this->mmTapeFollow->isActive()) {
    // 	bool following = !(this->mmTapeFollow->isTurning() ||
    //             this->mmTapeFollow->isSeeking());
    // 	bool passengerActive = this->mmPassengerSeek->isActive();
    // 	if (passengerActive && this->mmTapeFollow->isTurning())
    // 	    this->mmPassengerSeek->stop();
    // 	else if (passengerActive && this->mmTapeFollow->isSeeking())
    // 	    this->mmPassengerSeek->pause();
    // 	else if ((!passengerActive) && following)
    // 	    this->mmPassengerSeek->start();
    // }

    // if (this->mmPassengerSeek->isApproachingPassenger())
    // 	this->mmTapeFollow->setMotorSpeedPassengerSeek();
    // if (this->mmPassengerSeek->isAtPassenger())
    // 	this->mmTapeFollow->pause();

    // TODO: mitigate communication between minor modes
    // TODO: activate and deactivate modes as necessary
}


void MFindPassenger::start()
{

    // LCD.clear();
    // LCD.print("Starting");
    // LCD.setCursor(0, 1);
    // LCD.print("MFindPassenger");
    // delay(1000);

    MajorMode::start();

    // Start intial minor modes
    this->mmTapeFollow->start();
    this->mmCollisionWatch->start();

    // this->mmPassengerSeek->start();
    // this->mmArmControl->start();
}


// TODO
void MFindPassenger::test()
{
    // LCD.clear();
    // LCD.print("Testing");
    // LCD.setCursor(0, 1);
    // LCD.print("MFindPassenger");
    // delay(1000);

    MajorMode::test();
    this->mmTapeFollow->test();
    this->mmCollisionWatch->test();
}
