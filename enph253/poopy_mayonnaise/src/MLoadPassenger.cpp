///
// MLoadPassenger.cpp
//
#include "allminormodes.hpp"
#include "MLoadPassenger.hpp"

void MLoadPassenger::init()
{
    MajorMode::init();
    //TODO: reinitialize specific state variables
}

// TODO
MLoadPassenger::MLoadPassenger()
    : MajorMode()
{
  this->init();

  // TODO: initialize specific minor modes
  this->mmArmControl = new ArmControl;
  this->allMinorModes.push_back(mmArmControl);

  this->mmCollisionWatch = new CollisionWatch;
  this->allMinorModes.push_back(mmCollisionWatch);
}


MLoadPassenger::~MLoadPassenger(){}

//TODO
void MLoadPassenger::loop()
{
  if(this->mmCollisionWatch->collisionHasOccurred()){
    //TODO: figure out what to do other than panic
  }

  if(true/*this->mmArmControl->onLeft*/){
    this->mmArmControl->turnAndReach(true, true);
  }else{
    this->mmArmControl->turnAndReach(false, true);
  }
  if(this->mmArmControl->isHolding()){
    //switch major mode to MToDestination
  }else{
    //reposition
  }
  
}

void MLoadPassenger::start()
{
  MajorMode::start();

  //start minor modes
  this->mmArmControl->start();
  this->mmCollisionWatch->start();
}

//TODO
void MLoadPassenger::test()
{
  MajorMode::test();
}

