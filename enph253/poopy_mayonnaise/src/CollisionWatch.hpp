///
// CollisionWatch.hpp
//
// Minor mode for monitoring collision sensors
//
#ifndef COLLISION_WATCH_HPP
#define COLLISION_WATCH_HPP

#include <StandardCplusplus.h>
#include <vector>
#include "MinorMode.hpp"

using std::vector;

class CollisionWatch : public MinorMode
{

private:

    const int collisionDetectPeriod;  // number of consecutive reads to detect a collision
    const vector<int> sensorPins;     // left, front, right, back

    vector<bool> pinReadings;
    vector<bool> collisionDetected;   // true if a collision has been detected

    vector<int> numCollisionReads;    // number of consecutive times  a collision has been read

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
