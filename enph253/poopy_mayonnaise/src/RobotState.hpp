///
// RobotState.hpp
//
// Class definition to hold the state of the robot.
// That is, the major mode, minor/failure modes, as well as control variables
//
#ifndef ROBOT_STATE_HPP
#define ROBOT_STATE_HPP

#include "MajorMode.hpp"
#include "MinorMode.hpp"

class RobotState {
public:
    RobotState();
    MajorMode& getMajorMode();
    MinorMode& getMinorModes();
};

#endif // ROBOT_STATE_HPP
