///
// CollisionWatch.hpp
//
// Minor mode for monitoring collision sensors
//
#ifndef COLLISION_WATCH_HPP
#define COLLISION_WATCH_HPP

#include "MinorMode.hpp"

class CollisionWatch : public MinorMode
{

private:

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
