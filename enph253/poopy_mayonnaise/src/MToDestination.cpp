///
// MToDestination.cpp
//
#include "MajorMode.hpp"
#include "MajorModeEnum.hpp"
#include "MToDestination.hpp"


void MToDestination::init()
{
    MajorMode::init();
}


MToDestination::MToDestination()
    : MajorMode(MajorModeEnum::TO_DESTINATION)
{
    // TODO
}


MToDestination::~MToDestination() {}


// TODO
void MToDestination::loop()
{
    MajorMode::loop();
}


// TODO
void MToDestination::start()
{
    MajorMode::start();
}


void MToDestination::test()
{
    MajorMode::test();
}
