///
// CollisionWatch.cpp
//
#include <StandardCplusplus.h>
#include <phys253.h>
#include "pins.hpp"
#include "CollisionWatch.hpp"


const int COLLISION_DETECT_PERIOD {5};
const int *CollisionWatch::sensorPins {pins::COLLISION_SENSORS};


void CollisionWatch::init()
{
    MinorMode::init();
    
    this->collisionDetected.reset();  // bits reset to 0000
    for (int i(0); i < 4; ++i)
	this->numCollisionReads[i] = 0;

    for (int i(0); i < 4; ++i)
	pinMode(pins::COLLISION_SENSORS[i], INPUT);
}


CollisionWatch::CollisionWatch()
    : MinorMode(),
      collisionDetectPeriod(COLLISION_DETECT_PERIOD)
{
    this->init();
}


CollisionWatch::~CollisionWatch() {}


void CollisionWatch::loop()
{
    bool collisionSeen(false);
    
    // Read collision sensors and update numReads array
    for (int i(0); i < 4; ++i) {
    	if (!(digitalRead(CollisionWatch::sensorPins[i]))) {
    	    this->numCollisionReads[i] = 0;
    	} else if (this->numCollisionReads[i] < this->collisionDetectPeriod) {
    	    this->numCollisionReads[i] += 1;
    	    collisionSeen = true;
    	} else {
    	    collisionSeen = true;
    	}
    }

    if (collisionSeen) {
	LCD.clear();
	LCD.print("COLLISION");
	delay(100);
    } else {
	LCD.clear();
	LCD.print("NO COLLISION");
	delay(100);
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
