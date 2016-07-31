#include <phys253.h>    
#include <LiquidCrystal.h>
#include "CollisionWatch.hpp"
#include "pins.hpp"

//const int COLLISION_SENSORS[]  { 4,  5};  // left front right back
const int collisionDetectPeriod {5};
const int numSensorsPin = sizeof(COLLISION_SENSORS);
int numCollisionReads[numSensorsPin];
bool collisionDetected[numSensorsPin];
int collisionDirection;

void collisionLoop(){
  // Read collision sensors and update numReads array
  collisionDirection = -1;
  
  for (int i(0); i < numSensorsPin; ++i) {
    if (!(digitalRead(COLLISION_SENSORS[i]))) 
        numCollisionReads[i] = 0;
    else if (numCollisionReads[i] < collisionDetectPeriod) 
        numCollisionReads[i] += 1;
  }

  // Check if number of collisions has reached detect period
  for (int i(0); i < numSensorsPin; ++i){
    if (numCollisionReads[i] >= collisionDetectPeriod){
        collisionDetected[i] = 1;
        collisionDirection = i;
    }
    else
        collisionDetected[i] = 0;
  }
}

bool hasDetectedCollision(){
  return collisionDetected[0] || collisionDetected[1];
}

