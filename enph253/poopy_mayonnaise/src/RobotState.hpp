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

    bool active;                                // whether the robot is active
    vector< const MajorMode* >  allMajorModes;        // all possible major modes
    vector< const MinorMode* >  allMinorModes;        // all possible minor modes
    MajorMode*                  activeMajorMode;      // pointer to currently active major mode
    vector< const MinorMode* >  activeMinorModes;     // vector of pointers to currently acive minor modes

    /*
     * Initialize the allMajorModes member with all necessary major modes.
     * Fill activeMajorMode with the first major mode
     */
    void initMajorModes();

    /*
     * Deallocate all major mode objects
     */
    void destroyMajorModes();

    /*
     * Initialize the allMinorModes member with all necessary minor modes.
     * Fill activeMinorModes with the initially-active minor modes.
     */
    void initMinorModes();

    /*
     * Deallocate all minor mode objects
     */
    void destroyMinorModes();

    /*
     * (Re)initializes any state variables
     */
    void init();

public:

    RobotState();   // constructor

    ~RobotState();  // deconstructor

    /*
     * Return true if the robot is currently active (i.e. looping through 
     * modes)
     */
    bool isActive();

    /*
     * Returns a pointer to the currently active major mode
     */
    MajorMode* getMajorMode();

    /*
     * Returns a reference to the vector of currently active minor modes
     */
    vector< const MinorMode* >& getMinorModes();

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

};

#endif // ROBOT_STATE_HPP
