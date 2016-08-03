///
// TapeFollow.hpp
//
// Minor mode for following tape
//
#ifndef TAPE_FOLLOW_HPP
#define TAPE_FOLLOW_HPP

#include <StandardCplusplus.h>
#include <bitset>
#include <vector>
#include "pins.hpp"
#include "Direction.hpp"

using std::vector;
using std::bitset;


enum class TFAction
{
    FOLLOWING,
    REVERSING,
    SEEKING,
    TURNING
};


namespace TapeFollow
{

    /*
     * Set all instance variables to their default starting values
     */
    void init();

    /*
     * Main loop funciton to follow tape. Uses `seekTape`, `followTape`,
     * and `makeTurn` to control behavior.
     */
    void loop();

    /*
     * Begin the loop
     */
    void start();

    /*
     * Stop the loop, but keep the current state of all variables.
     */
    void pause();

    /*
     * Enter a mode where readings are still made, but motors are always
     * at speed 0.
     */
    void test();

    /*
     * Print output to the LCD
     */
    void printLCD();

    /*
     * Changes the motor following speed to the given value
     */
    void setMotorSpeedPassengerSeek();  // TODO: find a better way to do this

    /*
     * Resets the following speed to the default value
     */
    void resetMotorSpeed();  // TODO: find a better way to do this

    /*
     * Turn around procedure
     */
    void turnAround();

    /*
     * Return true if the robot is currently turning; else false
     */
    bool isTurning();

    /*
     * Return true if the robot is currently seeking tape
     */
    bool isSeeking();

    /*
     * Give weighted directions for turning. Robo will turn at next intersection randomly,
     * but weighted by ratio of parameters. If you want it to only turn in one direction, 
     * give it 0 for the others. If it can't turn in a nonzero direction, it will turn randomly
     * in one of the directions it can go.
     */
    void giveTurnDirection(float left, float right, float straight);

}

#endif // TAPE_FOLLOW_HPP
