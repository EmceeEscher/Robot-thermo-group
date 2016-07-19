///
// control.hpp
//
// Main file for controlling the various modes and behaviors of the robot
//
#ifndef CONTROL_HPP
#define CONTROL_HPP


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
     * Function that halts the loop and discards the current state
     * of all modes
     */
    void stop();

    /*
     * Function that halts the loop and maintains the current state
     * of all modes
     */
    void pause();

}

#endif // CONTROL_HPP
