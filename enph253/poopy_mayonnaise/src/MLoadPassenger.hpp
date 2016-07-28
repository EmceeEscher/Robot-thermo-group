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
    CollisionWatch  *mmCollisionWatch;
    PassengerSeek   *mmPassengerSeek;

    MajModeEnum state;
    
    /*
     * (Re)initialize state variables
     */
    void init();

public:

    MLoadPassenger(
      ArmControl      *mmArmControl,
      PassengerSeek   *mmPassengerSeek,
      CollisionWatch  *mmCollisionWatch);   // constructor

    ~MLoadPassenger();  // deconstructor

    void loop();

    void start();

    void test();

    /*
     * Used to tell if the robot should switch robot states or not
     */
    MajModeEnum changeTo();

};

#endif  // M_LOAD_PASSENGER
