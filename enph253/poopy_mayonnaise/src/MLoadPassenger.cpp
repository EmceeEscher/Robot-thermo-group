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

  this->mmPassengerSeek = new PassengerSeek;
  this->allMinorModes.push_back(mmPassengerSeek);
}


MLoadPassenger::~MLoadPassenger(){}

//TODO
void MLoadPassenger::loop()
{
  if(this->mmCollisionWatch->collisionHasOccurred()){
    //TODO: figure out what to do other than panic
  }

  if(this->mmPassengerSeek->isAtPassenger()){
    if(this->mmPassengerSeek->getPassengerSide() == 1){
      this->mmArmControl->turnAndReach(true, true);
    }else if(this->mmPassengerSeek->getPassengerSide() == -1){
      this->mmArmControl->turnAndReach(false, true);
    }
    if(this->mmArmControl->isHolding()){
    //switch major mode to MToDestination
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
}

//TODO
void MLoadPassenger::test()
{
  MajorMode::test();
}

