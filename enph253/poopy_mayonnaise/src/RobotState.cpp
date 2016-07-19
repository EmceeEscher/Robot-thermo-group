///
// RobotState.cpp
//
#include <StandardCplusplus.h>
#include <vector>
#include "MajorMode.hpp"
#include "MinorMode.hpp"
#include "modes.hpp"
#include "RobotState.hpp"


// TODO
void RobotState::initMajorModes()
{
}


// TODO
void RobotState::destroyMajorModes()
{
    for (const MajorMode *mm : this->allMajorModes)
	delete mm;
}


// TODO
void RobotState::initMinorModes()
{
    // constant pointer to non-constant data
    const MinorMode *tapeFollow = new TapeFollow3;
    this->allMinorModes.push_back(tapeFollow);
}


// TODO
void RobotState::destroyMinorModes()
{
    for (const MinorMode *mm : this->allMinorModes)
    	delete mm;
}


// TODO
RobotState::RobotState()
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


MajorMode* RobotState::getMajorMode()
{
    return this->activeMajorMode;
}


std::vector< const MinorMode* >& RobotState::getMinorModes()
{
    return this->activeMinorModes;
}


// TODO
void RobotState::loop()
{
}


// TODO
void RobotState::start()
{
    this->active = true;
}


// TODO
void RobotState::stop()
{
    this->active = false;
    this->init();
}


// TODO
void RobotState::pause()
{
    this->active = false;
}
