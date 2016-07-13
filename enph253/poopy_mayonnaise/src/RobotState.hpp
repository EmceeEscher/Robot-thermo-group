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
    bool active;                              // whether the robot is active
    std::vector<MajorMode> allMajorModes;     // all possible major modes
    std::vector<MinorMode> allMinorModes;     // all possible minor modes
    MajorMode activeMajorMode;                // currently active major mode
    std::vector<MinorMode> activeMinorModes;  // currently active minor modes
public:
    RobotState();

    /*
     * Return true if the robot is currently active (i.e. looping through 
     * modes)
     */
    bool isActive();

    /*
     * Returns a reference to the currently active major mode
     */
    MajorMode& getMajorMode();

    /*
     * Returns a reference to the vector of currently active minor modes
     */
    std::vector<MinorMode>& getMinorModes();
};

#endif // ROBOT_STATE_HPP
