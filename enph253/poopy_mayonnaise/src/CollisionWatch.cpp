///
// CollisionWatch.cpp
//
#include <phys253.h>
#include "pins.hpp"
#include "CollisionWatch.hpp"


const int COLLISION_DETECT_PERIOD {5};


// TODO
void CollisionWatch::init()
{
    MinorMode::init();

    this->pinReadings       = {false, false, false, false};
    this->collisionDetected = {false, false, false, false};

    this->numCollisionReads = {0, 0, 0, 0};
}


// TODO
CollisionWatch::CollisionWatch()
    : MinorMode(),
      collisionDetectPeriod(COLLISION_DETECT_PERIOD),
      sensorPins(pins::COLLISION_SENSORS)
{
}


CollisionWatch::~CollisionWatch() {}


// TODO
void CollisionWatch::loop()
{
    // Read collision sensors and update numReads array
    for (int i(0); i < 4; ++i) {
	if (static_cast<bool>(digitalRead(this->sensorPins[i])))
	    this->numCollisionReads[i] += 1;
	else 
	    this->numCollisionReads[i] = 0;
    }

    // Check if number of collisions has reached detect period
    for (int i(0); i < 4; ++i)
	this->collisionDetected[i] =
	        (this->numCollisionReads[i] >= this->collisionDetectPeriod);
}


// TODO
void CollisionWatch::test()
{
    MinorMode::test();
}


bool CollisionWatch::collisionHasOccurred()
{
    for (bool b : this->collisionDetected)
	if (b)
	    return true;
    return false;
}


int CollisionWatch::collisionDirection()
{
    for (int i(0); i < 4; ++i)
	if (this->collisionDetected[i])
	    return i;
    return -1;
}
