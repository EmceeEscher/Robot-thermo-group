///
// MLoadPassenger.hpp
//
// Major mode for loading passengers
//
#ifndef M_LOAD_PASSENGER_HPP
#define M_LOAD_PASSENGER_HPP

#include "MajorMode.hpp"


class MLoadPassenger : public MajorMode
{

private:

    bool active;

    /*
     * (Re)initialize state variables
     */
    void init();

public:

    MLoadPassenger();   // constructor

    ~MLoadPassenger();  // deconstructor

    /*
     * Major loop function for handling associated minor modes
     */
    void loop();

    /*
     * Make active (i.e. begin looping)
     */
    void start();

    /*
     * Make inactive (i.e. halt looping), and discard current state
     */
    void stop();

    /*
     * Make inactive, but keep current state
     */
    void pause();

    /*
     * Return true if the mode is currently active
     */
    bool isActive();

    /*
     * Enter the next major mode
     */
    void nextMode();

};

#endif  // M_LOAD_PASSENGER
