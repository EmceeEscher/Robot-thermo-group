///
// RobotState.cpp
//
#include <StandardCplusplus.h>
#include <vector>
#include <phys253.h>
#include "MajorMode.hpp"
#include "MinorMode.hpp"
#include "modes.hpp"
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
    // MAJOR MODES
    this->mFindPassenger = new MFindPassenger;
    this->allMajorModes.push_back(this->mFindPassenger);

    // this->mLoadPassenger = new MLoadPassenger;
    // this->allMajorModes.push_back(this->mLoadPassenger);

    // this->mToDestination = new MToDestination;
    // this->allMajorModes.push_back(this->mToDestination);

    // this->mDropPassenger = new MDropPassenger;
    // this->allMajorModes.push_back(this->mDropPassenger);

    this->init();
}


// TODO
RobotState::~RobotState()
{
    for (MajorMode *mm : this->allMajorModes)
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
    this->active = true;
    this->currentMajorMode->start();
}


// TODO
void RobotState::stop()
{
    this->active = false;
    this->init();
    for (auto *m : this->allMajorModes)
	if (m->isActive())
	    m->stop();
}


// TODO
void RobotState::pause()
{
    this->active = false;
    // for now, pause all active minor modes
    for (auto *m : this->allMajorModes)
	if (m->isActive())
	    m->pause();
}


void RobotState::test()
{
    this->active = true;
    // for now, test all minor modes
    for (auto *m : this->allMajorModes)
	if (m->isActive())
	    m->test();
}
