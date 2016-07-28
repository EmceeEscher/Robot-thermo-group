///
// MLoadPassenger.cpp
//
#include "allminormodes.hpp"
#include "MLoadPassenger.hpp"
#include <phys253.h>

void MLoadPassenger::init()
{
    MajorMode::init();
    //TODO: reinitialize specific state variables
}

// TODO
MLoadPassenger::MLoadPassenger(
  ArmControl      *mmArmControl,
  PassengerSeek   *mmPassengerSeek,
  CollisionWatch  *mmCollisionWatch)
    : MajorMode(),
    mmArmControl(mmArmControl),
    mmPassengerSeek(mmPassengerSeek),
    mmCollisionWatch(mmCollisionWatch)
{
  this->init();

  // TODO: initialize specific minor modes
  this->allMinorModes.push_back(mmArmControl);
  this->allMinorModes.push_back(mmCollisionWatch);
  this->allMinorModes.push_back(mmPassengerSeek);
}


MLoadPassenger::~MLoadPassenger(){}

//TODO
void MLoadPassenger::loop()
{
  if(this->mmCollisionWatch->collisionHasOccurred()){
    //TODO: figure out what to do other than panic
    //UPDATE: just fucking ignore it
  }

  if(this->mmPassengerSeek->isAtPassenger()){
    LCD.clear();
    if(this->mmPassengerSeek->getPassengerSide() == 1){
      LCD.print("reaching on right");
      this->mmArmControl->turnAndReach(true, true);
    }else if(this->mmPassengerSeek->getPassengerSide() == -1){
      LCD.print("reaching on left");
      this->mmArmControl->turnAndReach(false, true);
    }
    if(this->mmArmControl->isHolding()){
      this->stop();
    }
    else{
      //reposition
    }
  }
  else{
    //reposition
  }
  
}

void MLoadPassenger::start()
{
  MajorMode::start();

  //start minor modes
  this->mmArmControl->start();
  this->mmCollisionWatch->start();
  this->mmPassengerSeek->start();
}

//TODO
void MLoadPassenger::test()
{
  MajorMode::test();
}

