///
// RobotState.cpp
//
#include <StandardCplusplus.h>
#include <vector>
#include "MajorMode.hpp"
#include "MinorMode.hpp"
#include "RobotState.hpp"


// TODO
RobotState::RobotState()
{
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
