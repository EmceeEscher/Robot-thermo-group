///
// MDropPassenger.hpp
//
// Major mode for unloading passengers
//
#ifndef M_DROP_PASSENGER_HPP
#define M_DROP_PASSENGER_HPP

#include "allminormodes.hpp"
#include "MajorMode.hpp"

class MDropPassenger : public MajorMode
{

private:

    // Named minor modes
    ArmControl      *mmArmControl;
    DetectBeacon    *mmDetectBeacon;
    CollisionWatch  *mmCollisionWatch;

    /*
     * (Re)initialize state variables
     */
    void init();

public:

    // constructor
    MDropPassenger(
            ArmControl      *mmArmControl,
	    DetectBeacon    *mmDetectBeacon,
	    CollisionWatch  *mmCollisionWatch
    );

    // deconstructor
    ~MDropPassenger();

    void loop();

    void start();

    void test();

};

#endif  // M_DROP_PASSENGER_HPP
