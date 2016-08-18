#include <phys253.h>    
#include <LiquidCrystal.h>
#include "CollisionWatch.hpp"
#include "pins.hpp"

const int collisionDetectPeriod {5};
const int numSensorsPin = sizeof(COLLISION_SENSORS)/sizeof(COLLISION_SENSORS[0]);
int numCollisionReads[numSensorsPin];
bool collisionDetected[numSensorsPin] = {false, false};
int collisionDirection;

void CollisionWatch::loop(){
    // Read collision sensors and update numReads array
    collisionDirection = -1;
    for (int i(0); i < numSensorsPin; ++i) {
        if (!(digitalRead(COLLISION_SENSORS[i]))) 
            numCollisionReads[i] = 0;
        else if (numCollisionReads[i] < collisionDetectPeriod) 
            numCollisionReads[i] += 1;
        if (numCollisionReads[i] >= collisionDetectPeriod){
            collisionDetected[i] = true;
            collisionDirection = i;
        }
        else
            collisionDetected[i] = false;
    }
}


bool CollisionWatch::hasDetectedCollision(){
    return collisionDetected[0];
}

