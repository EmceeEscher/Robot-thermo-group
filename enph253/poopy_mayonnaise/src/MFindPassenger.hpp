///
// MFindPassenger.hpp
//
// Major mode for finding passenger
//
#ifndef M_FIND_PASSENGER_HPP
#define M_FIND_PASSENGER_HPP

#include "allminormodes.hpp"
#include "MajorMode.hpp"

class MFindPassenger : public MajorMode
{

private:

    // Named minor modes
    TapeFollow     *mmTapeFollow;
    PassengerSeek  *mmPassengerSeek;
    CollisionWatch *mmCollisionWatch;

    /*
     * (Re)initialize state variables
     */
    void init();

public:

    MFindPassenger();   // constructor

    ~MFindPassenger();  // deconstructor

    void loop();

    void start();

    void test();

};


#endif  // M_FIND_PASSENGER_HPP