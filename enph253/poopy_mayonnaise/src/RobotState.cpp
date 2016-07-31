///
// RobotState.cpp
//
#include <phys253.h>
#include "allmajormodes.hpp"
#include "allminormodes.hpp"
#include "MajorModeEnum.hpp"
#include "RobotState.hpp"


const unsigned long MAIN_LOOP_DELAY {1};     // milliseconds


void RobotState::init()
{
    this->active = false;
    this->currentMajorMode = this->mFindPassenger;
    this->nextMajorMode    = this->mFindPassenger;
}


void RobotState::setNextMode()
{
    switch (this->currentMajorMode->changeTo()) {
    case MajorModeEnum::FIND_PASSENGER :
	this->nextMajorMode = this->mFindPassenger;
	break;
    case MajorModeEnum::LOAD_PASSENGER:
	this->nextMajorMode = this->mLoadPassenger;
	break;
    case MajorModeEnum::DROP_PASSENGER:
	this->nextMajorMode = this->mDropPassenger;
	break;
    case MajorModeEnum::TO_DESTINATION:
    	this->nextMajorMode = this->mToDestination;
    	break;
    }
}


void RobotState::enterNextMode()
{
    if (this->currentMajorMode->isActive()) {
	this->currentMajorMode->pause();
	this->nextMajorMode->start();
    }
    this->currentMajorMode = this->nextMajorMode;
}


// TODO
RobotState::RobotState()
    : numMajorModes(NUM_MAJOR_MODES),
      numMinorModes(NUM_MINOR_MODES),
      allMajorModes(NUM_MAJOR_MODES),
      allMinorModes(NUM_MINOR_MODES),
      active(false), mainLoopDelay(MAIN_LOOP_DELAY)
{
    // Declare minor modes as dynamic pointers
    TapeFollow     *mmTapeFollow     = new TapeFollow;
    PassengerSeek  *mmPassengerSeek  = new PassengerSeek;
    CollisionWatch *mmCollisionWatch = new CollisionWatch;
    ArmControl     *mmArmControl     = new ArmControl;
    DetectBeacon   *mmDetectBeacon   = new DetectBeacon;

    // Push minor mode pointers to allMinorModes vector
    this->allMinorModes[0] = mmTapeFollow;
    this->allMinorModes[1] = mmPassengerSeek;
    this->allMinorModes[2] = mmCollisionWatch;
    this->allMinorModes[3] = mmArmControl;
    this->allMinorModes[4] = mmDetectBeacon;
   
    // Declare major modes as dynamic pointers
    this->mFindPassenger = new MFindPassenger(
            mmArmControl, mmTapeFollow, mmPassengerSeek, mmCollisionWatch);
    this->mLoadPassenger = new MLoadPassenger(
            mmArmControl, mmPassengerSeek, mmCollisionWatch);
    this->mDropPassenger = new MDropPassenger(
            mmArmControl, mmDetectBeacon, mmCollisionWatch);
    this->mToDestination = new MToDestination;

    // Push major modes to allMajorModes vector
    this->allMajorModes[0] = this->mFindPassenger;
    this->allMajorModes[1] = this->mLoadPassenger;
    this->allMajorModes[2] = this->mDropPassenger;
    this->allMajorModes[3] = this->mToDestination;
      
    // initialization
    this->init();
}


// TODO
RobotState::~RobotState()
{
    for (auto *mm : this->allMinorModes)
	delete mm;
    for (auto *mm : this->allMajorModes)
	delete mm;
}


bool RobotState::isActive()
{
    return this->active;
}


// TODO
void RobotState::loop()
{
    // Loop current major mode, if active
    if (this->currentMajorMode->isActive())
	this->currentMajorMode->loop();

    // Enter next mode, if different than current
    this->setNextMode();
    if (this->nextMajorMode != this->currentMajorMode)
        this->enterNextMode();

    delay(this->mainLoopDelay);
}


// TODO
void RobotState::start()
{
    this->active = true;
    this->currentMajorMode->start();
}


// TODO
void RobotState::stop()
{
    this->active = false;
    for (auto *m : this->allMajorModes)
	m->stop();
    this->init();
}


// TODO
void RobotState::pause()
{
    this->active = false;
    // for now, pause all major modes
    for (auto *m : this->allMajorModes)
	m->pause();
}


void RobotState::test()
{
    this->active = true;
    this->currentMajorMode->test();
}
