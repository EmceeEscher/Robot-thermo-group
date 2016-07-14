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

using std::vector;

// TODO
class RobotState
{
private:
    bool active;                          // whether the robot is active
    vector<MajorMode>  allMajorModes;     // all possible major modes
    vector<MinorMode>  allMinorModes;     // all possible minor modes
    MajorMode*         activeMajorMode;   // ponter to currently active major mode
    vector<MinorMode*> activeMinorModes;  // vector of pointers to currently acive minor modes
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
    vector<MinorMode*>& getMinorModes();

    /*
     * Loops through all of the currently active modes
     */
    void loop();
};

#endif // ROBOT_STATE_HPP
