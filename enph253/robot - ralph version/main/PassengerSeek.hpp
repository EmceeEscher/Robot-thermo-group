///
// PassengerSeek.hpp
//
// Seek passenger signals
//
#ifndef PASSENGER_SEEK_HPP
#define PASSENGER_SEEK_HPP


namespace PassengerSeek
{

    void init();

    /*
     * Main loop function to seek passengers. Halts the TapeFollow minor
     * mode when the passenger is directly adjacent to the robot
     */
    void loop();
    
    /*
     * Activates passenger seeking
     */
    void start();
    
    /*
     * Resets the state variables and pauses the mode.
     */
    void stop();
    
    /*
     * Deactivates the mode but maintains the current state.
     */
    void pause();
    
    /*
     * Enter a mode where readings are still made, but hardware (i.e. motors)
     * are not active
     */
    void test();
    
    /*
     * Return true if active
     */
    bool isActive();
    
    /*
     * Returns true if one of the front side pins is at a maximum
     */
    bool isApproachingPassenger();
    
    /*
     * Returns true if the middle (arm) pin is at a maximum after, the
     * front maximum was seen
     */
    bool isAtPassenger();
    
    /*
     * If at passenger, returns 1 if the passenger is on the right and -1
     * if the passenger is on the left.
     * Behavior is undefined if we are not at a passenger.
     */
    int getPassengerSide();

    /*
     * prints the values read by the passenger IR sensors on the LCD screen
     */
    void printLCD();


    /*
     * Picks a direction to turn at the next intersection based on whether
     * or not the front QSDs can see an IR beacon
     */
    void pickDirection();

    /*
     * Returns true if the the robot has reached (and passed) a maximum value for one of the front IR sensors
     */
    bool atMaxSideFront();

    /*
     * Returns true if the robot has reached (and passed) a maximum value for one of the side IR sensors
     */
    bool atMaxSideMiddle();

    /*
     * Updates whether or not the IR sensors have reached a max value 
     */
    void updateMax();
    
}

#endif  // PASSENGER_SEEK_HPP
