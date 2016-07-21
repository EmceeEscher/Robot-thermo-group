///
// MFindPassenger.cpp
//
#include <phys253.h>
#include "MinorMode.hpp"
#include "allminormodes.hpp"
#include "MFindPassenger.hpp"


// TODO
void MFindPassenger::init()
{
    MajorMode::init();
    // TODO: reinitialize specific state variables
}


// TODO
MFindPassenger::MFindPassenger()
    : MajorMode()
{
    // TODO: initialize specific minor modes
    this->mmTapeFollow = new TapeFollow;
    this->allMinorModes.push_back(mmTapeFollow);

    this->mmPassengerSeek = new PassengerSeek;
    this->allMinorModes.push_back(mmPassengerSeek);

    this->mmCollisionWatch = new CollisionWatch;
    this->allMinorModes.push_back(mmCollisionWatch);
}


MFindPassenger::~MFindPassenger() {}


// TODO
void MFindPassenger::loop()
{
    MajorMode::loop();  // does loop for each active minor mode

    // only seek passengers when not turning or seeking
    if (this->mmTapeFollow->isActive()) {
	bool following = !(this->mmTapeFollow->isTurning() ||
                this->mmTapeFollow->isSeeking());
	bool passengerActive = this->mmPassengerSeek->isActive();
	if (passengerActive && this->mmTapeFollow->isTurning())
	    this->mmPassengerSeek->stop();
	else if (passengerActive && this->mmTapeFollow->isSeeking())
	    this->mmPassengerSeek->pause();
	else if ((!passengerActive) && following)
	    this->mmPassengerSeek->start();
    }

    // TODO: mitigate communication between minor modes
    // TODO: activate and deactivate modes as necessary
}


void MFindPassenger::start()
{
    // TODO: remove this junk
    LCD.clear();
    LCD.print("Starting MI");
    delay(1000);

    MajorMode::start();
    
    // Start intial minor modes
    this->mmTapeFollow->start();
    this->mmCollisionWatch->start();
}


// TODO
void MFindPassenger::test()
{
    MajorMode::test();  // default major mode method
    this->mmTapeFollow->test();
}
