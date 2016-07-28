///
// MFindPassenger.cpp
//
#include "allminormodes.hpp"
#include "MFindPassenger.hpp"


// TODO
void MFindPassenger::init()
{
    MajorMode::init();
    // TODO: reinitialize specific state variables
}


// TODO
MFindPassenger::MFindPassenger(
        TapeFollow     *mmTapeFollow,
	PassengerSeek  *mmPassengerSeek,
	CollisionWatch *mmCollisionWatch
)
    : MajorMode(),
      mmTapeFollow(mmTapeFollow),
      mmPassengerSeek(mmPassengerSeek),
      mmCollisionWatch(mmCollisionWatch),
      state(MajModeEnum::DontChange)
{
    this->init();

    // TODO: initialize specific minor modes
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

    // // only seek passengers when not turning or seeking
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

    // TODO: mitigate communication between minor modes
    // TODO: activate and deactivate modes as necessary
}


void MFindPassenger::start()
{
    MajorMode::start();

    // Start intial minor modes
    this->mmTapeFollow->start();
    this->mmCollisionWatch->start();
    // this->mmPassengerSeek->start();
    this->state = MajModeEnum::DontChange;
}


// TODO
void MFindPassenger::test()
{
    MajorMode::test();
    this->mmTapeFollow->test();
    this->mmCollisionWatch->test();
}

MajModeEnum MFindPassenger::changeTo()
{
    return state;
}

