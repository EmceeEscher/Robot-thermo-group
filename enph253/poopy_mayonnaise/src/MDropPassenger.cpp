///
// MDropPassenger.cpp
//
#include "allminormodes.hpp"
#include "MDropPassenger.hpp"


//TODO
void MDropPassenger::init()
{
    MajorMode::init();
    // TODO: reinitialize specific state variables
}

//TODO
MDropPassenger::MDropPassenger(
  ArmControl      *mmArmControl,
  DetectBeacon    *mmDetectBeacon,
  CollisionWatch  *mmCollisionWatch)
    : MajorMode(),
    mmArmControl(mmArmControl),
    mmDetectBeacon(mmDetectBeacon),
    mmCollisionWatch(mmCollisionWatch),
    state(MajModeEnum::DontChange)
{
  this->init();

  // TODO: initialize specific minor modes
  this->allMinorModes.push_back(mmArmControl);
  this->allMinorModes.push_back(mmDetectBeacon);
  this->allMinorModes.push_back(mmCollisionWatch);
}

MDropPassenger::~MDropPassenger(){}

// TODO

void MDropPassenger::loop()
{
    MajorMode::loop();

    if(this->mmDetectBeacon->hasArrived())
    {
        int dropoffDir = this->mmDetectBeacon->getBeaconDirection();
        if(dropoffDir == 1){
          this->mmArmControl->turnAndReach(true, false);
        }else if(dropoffDir == -1){
          this->mmArmControl->turnAndReach(false, false);
        }
    }
    if(!this->mmArmControl->isHolding()){
      this->state = MajModeEnum::FindPassenger;
    }
}

void MDropPassenger::start()
{
    MajorMode::start();

    this->mmArmControl->start();
    this->mmCollisionWatch->start();
    this->mmDetectBeacon->start();
    this->state = MajModeEnum::DontChange;
}

// TODO
void MDropPassenger::test()
{
    MajorMode::test();

    this->mmArmControl->test();
    this->mmCollisionWatch->test();
    this->mmDetectBeacon->test();
}

MajModeEnum MDropPassenger::changeTo(){
    return this->state;
}

