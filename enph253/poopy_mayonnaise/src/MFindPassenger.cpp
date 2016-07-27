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
MFindPassenger::MFindPassenger()
    : MajorMode()
{
    this->init();

    // TODO: initialize specific minor modes
    this->mmTapeFollow = new TapeFollow;
    this->allMinorModes.push_back(mmTapeFollow);

    this->mmCollisionWatch = new CollisionWatch;
    this->allMinorModes.push_back(mmCollisionWatch);

    this->mmPassengerSeek = new PassengerSeek;
    this->allMinorModes.push_back(mmPassengerSeek);

}


MFindPassenger::~MFindPassenger() {}


// TODO
void MFindPassenger::loop()
{
    MajorMode::loop();  // does loop for each active minor mode

    // if (this->mmCollisionWatch->collisionHasOccurred()) {
    // 	// for now, just turn around
    // 	this->mmTapeFollow->turnAround();
    // }

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
    // this->mmCollisionWatch->start();
    // this->mmPassengerSeek->start();
}


// TODO
void MFindPassenger::test()
{
    MajorMode::test();

    this->mmTapeFollow->test();
}
