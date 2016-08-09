#ifndef ARM_AND_STEPPER_HPP
#define ARM_AND_STEPPER_HPP

#include <phys253.h>
#include <phys253pins.h>

namespace ArmAndStepper{

    /*
     * Sets initial values for the arm and the stepper motor
     */
    void init();
    
    /*
     * The control loop. Should be implemented wherever the code be for extended perids of time to prevent arm from overcorrecting,
     * grinding gears, etc. Should be repeated with several millisecond delays for integral/derivative control purposes
     */
    void doControl();

    /*
     * Converts base potentiometer voltage to corresponding angle
     */
    float getAngle();

    /*
     * Wrapper function for setting motor speed. Prevents values larger than 255 in either direction
     * @param duty: the speed at which you wish to set the motor
     */
    void setBaseMotor(long duty);

    /*
     * Returns the motor speed based on PID control
     */
    float getControlValue();

    /*
     * Closes the claw until something is detected in claw, the claw closes on itself, or a timeout is reached
     */
    void grab();

    /*
     * Opens the claw for specified time
     */
    void drop();

    /*
     * Tells arm to reach outward, and either grab or release with the claw, depending on the parameter.
     * Reaches in a piecemeal fashion, to avoid slamming the arm down and breaking it.
     * Grabbing with this function is now no longer used (use refinedReachAndGrab instead), but this function
     * is still used to drop
     * @param grabbing: true if you want the claw to grab, false if you want it to drop
     */
    void reachAndClaw(bool grabbing);

    /*
     * A better method for reaching and grabbing than using reachAndClaw(true)
     */
    void refinedReachAndGrab();

    /*
     * Returns the arm to its rest position, but gradually, so it doesn't whip backwards and break
     */
    void setRestPosition();

    /*
     * Tells the stepper motor to turn in the specified direction for the specified number of steps
     * @param CW: true for arm turning clockwise, false for arm turning counterclockwise
     * @param count: number of steps to turn (~700 is about 90 degrees)
     */
    void stepperTurn(bool CW,int count);

    /*
     * Tells the arm to turn 90 degrees, and either close or open the claw
     * @param turnRight: true to turn right, false to turn left
     * @param grab: true to close the claw, false to release
     */
    void turnAndReach(bool turnRight, bool grab);

    /*
     * returns true if the claw thinks it's holding something, false otherwise
     */
    bool isHolding();

    /*
     * Used to overwrite the holding variable if the rest of the program has determined that
     * the arm is not correctly detecting whether or not it's holding something
     */
    void setHolding(bool newHolding);

}

#endif //ARM_AND_STEPPER_HPP
