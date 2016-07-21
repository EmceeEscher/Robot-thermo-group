///
// CollisionWatch.hpp
//
// Minor mode for monitoring collision sensors
//
#ifndef COLLISION_WATCH_HPP
#define COLLISION_WATCH_HPP

#include <StandardCplusplus.h>
#include <vector>
#include "Direction.hpp"
#include "MinorMode.hpp"

using std::vector;

class CollisionWatch : public MinorMode
{

private:

    const vector<int> sensorPins;  // left, front, right, back

    bool collision;  // true if collision just occurred
    Direction dir;   // direction of collision reading

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

};

#endif  // COLLISION_WATCH_HPP
