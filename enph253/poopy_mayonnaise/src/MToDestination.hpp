///
// MToDestination.hpp
//
// Major mode for navigating to the dropoff location
//
#ifndef M_TO_DESTINATION_HPP
#define M_TO_DESTINATION_HPP

#include "MajorMode.hpp"
#include "allminormodes.hpp"

class MToDestination : public MajorMode
{

private:

    //Named minor modes
    ArmControl      *mmArmControl;
    CollisionWatch  *mmCollisionWatch;
    DetectBeacon    *mmDetectBeacon;
    TapeFollow      *mmTapeFollow;
    
    bool active;

    int diffThreshold;
    /*
     * (Re)initialize state variables
     */
    void init();

public:

    MToDestination(
      ArmControl      *mmArmControl,
      CollisionWatch  *mmCollisionWatch,
      DetectBeacon    *mmDetectBeacon,
      TapeFollow      *mmTapeFollow
      );

    ~MToDestination();  // deconstructor

    void loop();

    void start();

    void test();

    /*
     * picks a direction to turn at the next intersection based on QSD readings
     * and sends it to the TapeFollow minor mode
     */
    void pickDirection();

};

#endif  // M_TO_DESTINATION_HPP
