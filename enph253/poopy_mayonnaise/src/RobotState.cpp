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
void RobotState::initMinorModes()
{
}


// TODO
RobotState::RobotState()
{
    this->initMajorModes();
    this->initMinorModes();
}


bool RobotState::isActive()
{
    return this->active;
}


MajorMode& RobotState::getMajorMode()
{
    return *this->activeMajorMode;
}


std::vector<MinorMode*>& RobotState::getMinorModes()
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
