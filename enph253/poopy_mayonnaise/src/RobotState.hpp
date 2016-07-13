///
// RobotState.hpp
//
// Class definition to hold the state of the robot.
// That is, the major mode, minor/failure modes, as well as control variables
//
#ifndef ROBOT_STATE_HPP
#define ROBOT_STATE_HPP

#include <StandardCplusplus.h>
#include <vector>
#include "MajorMode.hpp"
#include "MinorMode.hpp"

// TODO
class RobotState {
private:
    MajorMode majorMode;
    std::vector<MinorMode>& minorModes;
public:
    RobotState();
    MajorMode& getMajorMode();
    std::vector<MinorMode>& getMinorModes();
};

#endif // ROBOT_STATE_HPP
