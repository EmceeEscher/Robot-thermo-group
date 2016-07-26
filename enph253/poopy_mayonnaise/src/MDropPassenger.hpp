///
// MDropPassenger.hpp
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

    void loop();

    void start();

    void stop();

    void pause();

    void test();

    bool isActive();
};

#endif  // M_DROP_PASSENGER_HPP
