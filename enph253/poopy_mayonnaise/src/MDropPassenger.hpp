///
// MUnloadPassenger.hpp
//
// Major mode for unloading passengers
//
#ifndef M_DROP_PASSENGER_HPP
#define M_DROP_PASSENGER_HPP

#include "MajorMode.hpp"


class MDropPassenger : public MajorMode
{

private:

    /*
     * (Re)initialize state variables
     */
    void init();

public:

    MDropPassenger();   // constructor

    ~MDropPassenger();  // deconstructor

    /*
     * Major loop function for handling associated minor modes
     */
    void loop();

    /*
     * Enter a testing mode, in which hardware is not active
     */
    void test();
};

#endif  // M_DROP_PASSENGER_HPP
