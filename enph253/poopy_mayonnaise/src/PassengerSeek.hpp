///
// PassengerSeek.hpp
//
// Minor mode for seeking passenger signals
//
#ifndef PASSENGER_SEEK_HPP
#define PASSENGER_SEEK_HPP

#include "MinorMode.hpp"

class PassengerSeek : public MinorMode
{

private:

    bool active;  // whether the loop is active

    /*
     * (Re)initialize all state variables
     */
    void init();

public:
    
    PassengerSeek();   // constructor

    ~PassengerSeek();  // deconstructor

    /*
     * Main loop function to seek passengers. Halts the TapeFollow minor
     * mode when the passenger is directly adjacent to the robot
     */
    void loop();

    /*
     * Begin the loop
     */
    void start();

    /*
     * Stop the loop, and reset all variables
     */
    void stop();

    /*
     * Stop the loop, but keep the current state of all variables
     */
    void pause();

    /*
     * Enter a mode where readings are still made, but hardware (i.e. motors)
     * are not active
     */
    void test();

    /*
     * Return true if the mode is currently active, else false
     */
    bool isActive();
    
};

#endif  // PASSENGER_SEEK_HPP
