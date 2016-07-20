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


// TODO
void RobotState::initMajorModes()
{
}


// TODO
void RobotState::destroyMajorModes()
{
    for (MajorMode *mm : this->allMajorModes)
	delete mm;
}


// TODO
void RobotState::initMinorModes()
{
    MinorMode *tapeFollow = new TapeFollow;
    this->allMinorModes.push_back(tapeFollow);

    MinorMode *passengerSeek = new PassengerSeek;
    this->allMinorModes.push_back(passengerSeek);
}


// TODO
void RobotState::destroyMinorModes()
{
    for (MinorMode *mm : this->allMinorModes)
    	delete mm;
}


// TODO
RobotState::RobotState()
    : mainLoopDelay(MAIN_LOOP_DELAY)
{
    this->initMajorModes();
    this->initMinorModes();
}


// TODO
RobotState::~RobotState()
{
    this->destroyMinorModes();
    this->destroyMajorModes();
}


bool RobotState::isActive()
{
    return this->active;
}


// TODO
void RobotState::loop()
{
    // for now, run each minor mode in no particular order
    for (auto *mm : this->allMinorModes)
	if (mm->isActive())
	    mm->loop();
    // TODO update activeMinorModes vector
    delay(this->mainLoopDelay);
}


// TODO
void RobotState::start()
{
    this->active = true;
    // for now, begin all minor modes
    for (auto *mm : this->allMinorModes)
	mm->start();
}


// TODO
void RobotState::stop()
{
    this->active = false;
    this->init();
    // for now, stop all active minor modes
    for (auto *mm : this->allMinorModes)
	if (mm->isActive())
	    mm->stop();
}


// TODO
void RobotState::pause()
{
    this->active = false;
    // for now, pause all active minor modes
    for (auto *mm : this->allMinorModes)
	if (mm->isActive())
	    mm->pause();
}


void RobotState::test()
{
    this->active = true;
    // for now, test all minor modes
    for (auto *mm : this->allMinorModes)
	mm->test();
}
