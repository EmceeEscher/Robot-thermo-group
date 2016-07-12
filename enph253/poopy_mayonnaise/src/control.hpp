///
// control.hpp
//
// Main file for controlling the various modes and behaviors of the robot
//
#ifndef CONTROL_HPP
#define CONTROL_HPP


#include <phys253.h>
#include "TapeFollow.hpp"


namespace control
{
    /*
     * Main setup function
     */
    void setup();
    
    /*
     * Main loop function to control the robot
     */
    void loop();

    /*
     * Function that begins the loop
     */
    void start();
    
    /*
     * Function that halts the loop
     */
    void stop();
}

#endif // CONTROL_HPP
