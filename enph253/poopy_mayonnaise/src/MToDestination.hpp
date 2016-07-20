///
// MToDestination.hpp
//
// Major mode for navigating to the dropoff location
//
#ifndef M_TO_DESTINATION_HPP
#define M_TO_DESTINATION_HPP

#include "MajorMode.hpp"


class MToDestination : public MajorMode
{

private:

    bool active;

    /*
     * (Re)initialize state variables
     */
    void init();

public:

    MToDestination();  // constructor

    ~MToDestination();  // deconstructor

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


#endif  // M_TO_DESTINATION_HPP
