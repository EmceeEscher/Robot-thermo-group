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
     * Enter a mode where readings are still made, but hardware (i.e. motors)
     * are not active
     */
    void test();
    
};

#endif  // PASSENGER_SEEK_HPP
