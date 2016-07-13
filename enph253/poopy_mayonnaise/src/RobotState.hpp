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
#include <map>
#include "MajorMode.hpp"
#include "MinorMode.hpp"

// TODO
class RobotState {
private:
    MajorMode activeMajorMode;                   // currently active major mode
    std::map<MinorMode, bool> activeMinorModes;  // map MinorMode->bool that tells whether the given mode is active
    std::vector<MajorMode> allMajorModes;        // all possible major modes
    std::vector<MinorMode> allMinorModes;        // all possible minor modes
public:
    RobotState();
    MajorMode& getMajorMode();
    std::vector<MinorMode>& getMinorModes();
};

#endif // ROBOT_STATE_HPP
