///
// MFindPassenger.hpp
//
// Major mode for finding passenger
//
#ifndef M_FIND_PASSENGER_HPP
#define M_FIND_PASSENGER_HPP

#include "MinorMode.hpp"
#include "MajorMode.hpp"


class MFindPassenger : public MajorMode
{

private:

    // Named minor modes
    MinorMode *mmTapeFollow;
    MinorMode *mmPassengerSeek;
    MinorMode *mmCollisionWatch;

    /*
     * (Re)initialize state variables
     */
    void init();

public:

    MFindPassenger();   // constructor

    ~MFindPassenger();  // deconstructor

    /*
     * Major loop function for handling associated minor modes
     */
    void loop();

    /*
     * Enter a testing mode, in which hardware is not active
     */
    void test();

};


#endif  // M_FIND_PASSENGER_HPP
