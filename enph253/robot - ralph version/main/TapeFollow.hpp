///
// TapeFollow.hpp
//
#ifndef TAPE_FOLLOW_HPP
#define TAPE_FOLLOW_HPP

#include "Direction.hpp"

namespace TapeFollow
{

    void init();

    void printLCD();
    
    void loop();
    
    void start();
    
    void stop();
    
    void pause();
    
    bool isActive();
    
    void test();
    
    void giveTurnDirection(float left, float right, float straight);

    void turnAround();

    void backUp();

}

#endif
