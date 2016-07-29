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
    ArmControl     *mmArmControl;
    TapeFollow     *mmTapeFollow;
    PassengerSeek  *mmPassengerSeek;
    CollisionWatch *mmCollisionWatch;

    MajModeEnum state;

    /*
     * (Re)initialize state variables
     */
    void init();

public:

    /*
     * Construct major mode with appropriate minor mode pointers.
     * These pointers are owned by the calling method, and it is its
     * responsibility to deallocate them.
     */
    MFindPassenger(
           ArmControl *mmArmControl,
           TapeFollow *mmTapeFollow,
           PassengerSeek *mmPassengerSeek,
	         CollisionWatch *mmCollisionWatch
    );   

    ~MFindPassenger();  // deconstructor

    void loop();

    void start();

    void test();

    MajModeEnum changeTo();

};


#endif  // M_FIND_PASSENGER_HPP
