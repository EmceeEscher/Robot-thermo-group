///
// MFindPassenger.cpp
//
#include "MinorMode.hpp"
#include "modes.hpp"
#include "MFindPassenger.hpp"


// TODO
void MFindPassenger::init()
{
    MajorMode::init();
    // TODO: reinitialize specific state variables
}


// TODO
MFindPassenger::MFindPassenger()
    : MajorMode()
{
    // TODO: initialize specific minor modes
    this->mmTapeFollow = new TapeFollow;
    this->allMinorModes.push_back(mmTapeFollow);

    this->mmPassengerSeek = new PassengerSeek;
    this->allMinorModes.push_back(mmPassengerSeek);

    this->mmCollisionWatch = new CollisionWatch;
    this->allMinorModes.push_back(mmCollisionWatch);
}


MFindPassenger::~MFindPassenger() {}


// TODO
void MFindPassenger::loop()
{
    MajorMode::loop();  // does loop for each active minor mode

    // TODO: mitigate communication between minor modes
    // TODO: activate and deactivate modes as necessary
}


// TODO
void MFindPassenger::test()
{
    MajorMode::test();  // default major mode method
}
