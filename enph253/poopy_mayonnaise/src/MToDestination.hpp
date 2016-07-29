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
    

    MajModeEnum state;
    
    bool active;
    
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

    MajModeEnum changeTo();

};

#endif  // M_TO_DESTINATION_HPP
