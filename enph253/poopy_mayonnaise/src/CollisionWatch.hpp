///
// CollisionWatch.hpp
//
// Minor mode for monitoring collision sensors
//
#ifndef COLLISION_WATCH_HPP
#define COLLISION_WATCH_HPP

#include <StandardCplusplus.h>
#include <bitset>
#include "pins.hpp"
#include "MinorMode.hpp"

using std::bitset;

class CollisionWatch : public MinorMode
{

private:

    static const int *sensorPins;            // left, front, right, back
    static const int numSensorPins = pins_sizes::COLLISION_SENSORS;

    const int collisionDetectPeriod;  // number of consecutive reads to detect a collision

    bitset<numSensorPins> collisionDetected;      // true if a collision has been detected
    int numCollisionReads[numSensorPins];         // number of consecutive times  a collision has been read

    /*
     * (Re)initializes all state variables
     */
    void init();

public:

    CollisionWatch();   // constructor

    ~CollisionWatch();  // deconstructor

    /*
     * Main loop function to watch for collisions. Halts the TapeFollow
     * minor mode when a collision is detected
     */
    void loop();

    /*
     * Enter a testing mode in which sensor readings are still made, but
     * hardware (i.e. motors) are not active
     */
    void test();

    /*
     * Returns true if a collision has been detected
     */
    bool collisionHasOccurred();

    /*
     * Returns the number associated with the direction where the collision
     * occurred. [0, 1, 2, 3] = [left, front, right, back]
     * Returns -1 if no collision has been detected.
     */
    int collisionDirection();

};

#endif  // COLLISION_WATCH_HPP
