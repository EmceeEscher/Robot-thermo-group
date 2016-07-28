///
// RobotState.cpp
//
#include <phys253.h>
#include "allmajormodes.hpp"
#include "allminormodes.hpp"
#include "RobotState.hpp"


const unsigned long MAIN_LOOP_DELAY {1};     // milliseconds


void RobotState::init()
{
    this->active = false;
    this->currentMajorMode = this->mFindPassenger;
}


// TODO
RobotState::RobotState()
    : mainLoopDelay(MAIN_LOOP_DELAY)
{
    // Minor modes
    TapeFollow *mmTapeFollow = new TapeFollow;
    this->allMinorModes.push_back(mmTapeFollow);


    PassengerSeek *mmPassengerSeek = new PassengerSeek;
    this->allMinorModes.push_back(mmPassengerSeek);


    CollisionWatch *mmCollisionWatch = new CollisionWatch;
    this->allMinorModes.push_back(mmCollisionWatch);
   
    // Major modes
    this->mFindPassenger = new MFindPassenger(
            mmTapeFollow,
	    mmPassengerSeek,
	    mmCollisionWatch
    );
    this->allMajorModes.push_back(this->mFindPassenger);
    
    // Initialization
    this->init();
}


// TODO
RobotState::~RobotState()
{
    for (auto *mm : this->allMajorModes)
	delete mm;
    for (auto *mm : this->allMinorModes)
	delete mm;
}


bool RobotState::isActive()
{
    return this->active;
}


// TODO
void RobotState::loop()
{
    // for now, just loop a single mode, without moving on to the next
    if (this->currentMajorMode->isActive())
	this->currentMajorMode->loop();
    delay(this->mainLoopDelay);
}


// TODO
void RobotState::start()
{
    // LCD.clear();
    // LCD.print("STARTING...");
    // delay(1000);
    this->active = true;
    this->currentMajorMode->start();
}


// TODO
void RobotState::stop()
{
    // LCD.clear();
    // LCD.print("STOPPED");
    // delay(1000);
    this->active = false;
    this->init();
    for (auto *m : this->allMajorModes)
	if (m->isActive())
	    m->stop();
}


// TODO
void RobotState::pause()
{
    // LCD.clear();
    // LCD.print("PAUSED");
    // delay(1000);
    this->active = false;
    // for now, pause active major mode
    for (auto *m : this->allMajorModes)
	if (m->isActive())
	    m->pause();
}


void RobotState::test()
{
    // LCD.clear();
    // LCD.print("TESTING...");
    // delay(1000);
    this->active = true;
    this->currentMajorMode->test();
}
