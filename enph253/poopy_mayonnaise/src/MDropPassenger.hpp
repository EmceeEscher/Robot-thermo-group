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
    CollisionWatch  *mmCollisionWatch;
    DetectBeacon    *mmDetectBeacon;


    MajModeEnum state;  
    /*
     * (Re)initialize state variables
     */
    void init();

public:

    MDropPassenger(
      ArmControl      *mmArmControl,
      DetectBeacon    *mmDetectBeacon,
      CollisionWatch  *mmCollisionWatch);   // constructor

    ~MDropPassenger();  // deconstructor

    void loop();

    void start();

    void test();

    /*
     * Used to tell if the robot should switch robot states or not
     */
    MajModeEnum changeTo();
};

#endif  // M_DROP_PASSENGER_HPP
