///
// PassengerSeek.hpp
//
// Seek passenger signals
//
#ifndef PASSENGER_SEEK_HPP
#define PASSENGER_SEEK_HPP

#include "pins.hpp"


namespace PassengerSeek
{

    /*
     * (Re)initialize all state variables
     */
    static void init();
    
    /*
     * Return true if one of the front side readings is at a maximum
     */
    static bool atMaxSideFront();
    
    /*
     * Return true if one of the middle side readings is at a maximum
     */
    static bool atMaxSideMiddle();
    
    /*
     * Update the atMax array to reflect the latest readings
     */
    static void updateMax();
    
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
    
}

#endif  // PASSENGER_SEEK_HPP
