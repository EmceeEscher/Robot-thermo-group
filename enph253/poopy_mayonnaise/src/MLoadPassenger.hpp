///
// MLoadPassenger.hpp
//
// Major mode for loading passengers
//
#ifndef M_LOAD_PASSENGER_HPP
#define M_LOAD_PASSENGER_HPP

#include "allminormodes.hpp"
#include "MajorMode.hpp"

class MLoadPassenger : public MajorMode
{

private:

    // Named minor modes
    ArmControl      *mmArmControl;
    PassengerSeek   *mmPassengerSeek;
    CollisionWatch  *mmCollisionWatch;
    
    /*
     * (Re)initialize state variables
     */
    void init();

public:

    // constructor
    MLoadPassenger(
            ArmControl      *mmArmControl,
	    PassengerSeek   *mmPassengerSeek,
	    CollisionWatch  *mmCollisionWatch
    );

    ~MLoadPassenger();  // deconstructor

    void loop();

    void start();

    void test();

};

#endif  // M_LOAD_PASSENGER
