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

    bool active;  // whether the loop is active

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
     * Begins the loop
     */
    void start();

    /*
     * Halts the loop and resets all state variables
     */
    void stop();

    /*
     * Halts the loop but maintains the current state
     */
    void pause();

    /*
     * Enter a testing mode in which sensor readings are still made, but
     * hardware (i.e. motors) are not active
     */
    void test();

    /*
     * Return true if the mode is currently active
     */
    bool isActive();
};

#endif  // COLLISION_WATCH_HPP
