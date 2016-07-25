///
// CollisionWatch.cpp
//
#include <StandardCplusplus.h>
#include <string>
#include <phys253.h>
#include "pins.hpp"
#include "CollisionWatch.hpp"


const int COLLISION_DETECT_PERIOD {5};


void CollisionWatch::init()
{
    MinorMode::init();

    this->collisionDetected.reset();  // bits reset to 0000
    for (int i(0); i < 4; ++i)
	this->numCollisionReads[i] = 0;
}


CollisionWatch::CollisionWatch()
    : MinorMode(),
      sensorPins(pins::COLLISION_SENSORS),
      collisionDetectPeriod(COLLISION_DETECT_PERIOD)
{
}


CollisionWatch::~CollisionWatch() {}


void CollisionWatch::loop()
{
    // Read collision sensors and update numReads array
    for (int i(0); i < 4; ++i) {
	if (digitalRead(this->sensorPins[i]))
	    this->numCollisionReads[i] += 1;
	else 
	    this->numCollisionReads[i] = 0;
    }

    // Check if number of collisions has reached detect period
    for (int i(0); i < 4; ++i)
	if (this->numCollisionReads[i] >= this->collisionDetectPeriod)
	    this->collisionDetected[i] = 1;
	else
	    this->collisionDetected[i] = 0;
}


// TODO
void CollisionWatch::test()
{
    MinorMode::test();
}


bool CollisionWatch::collisionHasOccurred()
{
    return this->collisionDetected.any();
}


int CollisionWatch::collisionDirection()
{
    for (int i(0); i < 4; ++i)
	if (this->collisionDetected[i])
	    return i;
    return -1;
}
