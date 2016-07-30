///
// RobotState.cpp
//
#include <phys253.h>
#include "allmajormodes.hpp"
#include "allminormodes.hpp"
#include "MajorModeEnum.hpp"
#include "RobotState.hpp"
#include "debug.hpp"


const unsigned long MAIN_LOOP_DELAY {
  1
};     // milliseconds


void RobotState::init()
{
  this->active = false;
  this->currentMajorMode = this->mFindPassenger;
  this->nextMajorMode    = this->mFindPassenger;
}


void RobotState::setNextMode()
{
  switch (this->currentMajorMode->changeTo()) {
    case MajorModeEnum::FIND_PASSENGER :
      this->nextMajorMode = this->mFindPassenger;
      break;
    case MajorModeEnum::LOAD_PASSENGER:
      this->nextMajorMode = this->mLoadPassenger;
      break;
    case MajorModeEnum::DROP_PASSENGER:
      this->nextMajorMode = this->mDropPassenger;
      break;
    case MajorModeEnum::TO_DESTINATION:
      this->nextMajorMode = this->mToDestination;
      break;
  }
}


void RobotState::enterNextMode()
{
  if (this->currentMajorMode->isActive()) {
    this->currentMajorMode->pause();
    this->nextMajorMode->start();
  }
  this->currentMajorMode = this->nextMajorMode;
}


// TODO
RobotState::RobotState()
  : mainLoopDelay(MAIN_LOOP_DELAY)
{

  // Minor modes
  TapeFollow *mmTapeFollow = new TapeFollow;
  this->allMinorModes[0] = mmTapeFollow;

  PassengerSeek *mmPassengerSeek = new PassengerSeek;
  this->allMinorModes[1] = mmPassengerSeek;

  CollisionWatch *mmCollisionWatch = new CollisionWatch;
  this->allMinorModes[2] = mmCollisionWatch;

  ArmControl *mmArmControl = new ArmControl;
  this->allMinorModes[3] = mmArmControl;

  DetectBeacon *mmDetectBeacon = new DetectBeacon;
  this->allMinorModes[4] = mmDetectBeacon;

  // Major modes
  this->mFindPassenger = new MFindPassenger(
    mmArmControl,
    mmTapeFollow,
    mmPassengerSeek,
    mmCollisionWatch
  );
  this->allMajorModes[0] = this->mFindPassenger;

  this->mLoadPassenger = new MLoadPassenger(
    mmArmControl,
    mmPassengerSeek,
    mmCollisionWatch
  );
  this->allMajorModes[1] = this->mLoadPassenger;

  this->mDropPassenger = new MDropPassenger(
    mmArmControl,
    mmDetectBeacon,
    mmCollisionWatch
  );
  this->allMajorModes[2] = this->mDropPassenger;
  
  // initialization
  this->init();
}


// TODO
RobotState::~RobotState()
{
  int i;
  for (i=0; i < 4; i++)
    delete allMajorModes[i];
  for (i=0; i < 5; i++)
    delete allMinorModes[i];
}


bool RobotState::isActive()
{
  return this->active;
}


// TODO
void RobotState::loop()
{
  // for now, just loop a single mode, without moving on to the next
  if (this->currentMajorMode->isActive())
    this->currentMajorMode->loop();

  // Enter next mode, if different than current
  this->setNextMode();
  if (this->nextMajorMode != this->currentMajorMode)
    this->enterNextMode();

  delay(this->mainLoopDelay);
}


// TODO
void RobotState::start()
{
  // LCD.clear();
  // LCD.print("STARTING...");
  // delay(1000);
  this->active = true;
  this->currentMajorMode->start();
}


// TODO
void RobotState::stop()
{
  // LCD.clear();
  // LCD.print("STOPPED");
  // delay(1000);
  this->active = false;
  this->init();
  int i;
  for(i = 0; i < 4; i++){
    if(allMajorModes[i]->isActive())
      allMajorModes[i]->stop();
  }
}


// TODO
void RobotState::pause()
{
  // LCD.clear();
  // LCD.print("PAUSED");
  // delay(1000);
  this->active = false;
  // for now, pause active major mode
  int i;
  for(i = 0; i < 4; i++){
    if(allMajorModes[i]->isActive()){
      allMajorModes[i]->pause();
    }
  }
}


void RobotState::test()
{
  // LCD.clear();
  // LCD.print("TESTING...");
  // delay(1000);
  this->active = true;
  this->currentMajorMode->test();
}
