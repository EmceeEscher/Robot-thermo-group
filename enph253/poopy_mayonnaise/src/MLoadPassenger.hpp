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
     * Enter a testing mode, in which hardware is not active
     */
    void test();

};

#endif  // M_LOAD_PASSENGER
