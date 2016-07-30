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

class RobotState
{

private:

    bool active;                         // whether the robot is active
    int mainLoopDelay;                   // delay for the main loop
    MajorMode* allMajorModes[4];  // all possible major modes
    MinorMode* allMinorModes[5];  // all possible minor modes
    MajorMode *currentMajorMode;         // current major mode
    MajorMode *nextMajorMode;

    // explicitly named major modes
    // TODO: Should these necessarily be degraded into their base?
    //       Do they need individual functionalities?
    MajorMode *mFindPassenger;
    MajorMode *mLoadPassenger;
    MajorMode *mToDestination;
    MajorMode *mDropPassenger;

    /*
     * (Re)initializes any state variables
     */
    void init();

    /*
     * Sets nextMajorMode based on the current major mode's `changeTo()`
     * method.
     */
    void setNextMode();

    /*
     * Enter the mode specified by `nextMajorMode`. If the current mode is
     * active, the next mode will also be active. Otherwise it will be
     * left inactive.
     *
     * NOTE: This method assumes nextMajorMode is different from
     * currentMajorMode. Calling it when they are the same will
     * cause the mode to restart if it is active.
     */
    void enterNextMode();

public:

    RobotState();   // constructor

    ~RobotState();  // deconstructor

    /*
     * Return true if the robot is currently active (i.e. looping through 
     * modes)
     */
    bool isActive();

    /*
     * Loops through all of the currently active modes
     */
    void loop();

    /*
     * Makes the robot actively loop
     */
    void start();

    /*
     * Halts the looping of the robot and reinitializes constants
     */
    void stop();

    /*
     * Halts the looping of the robot
     */
    void pause();

    /*
     * Cause all mode to enter a testing mode
     */
    void test();

};

#endif // ROBOT_STATE_HPP
