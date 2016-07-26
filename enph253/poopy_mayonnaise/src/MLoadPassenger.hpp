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

    void loop();

    void test();

};

#endif  // M_LOAD_PASSENGER
