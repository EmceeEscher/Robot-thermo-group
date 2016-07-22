///
// MinorMode.cpp
//
#include <phys253.h>
#include "MinorMode.hpp"


void MinorMode::init()
{
    this->active = false;
}


MinorMode::MinorMode()
    : active(false)
{
    this->init();
}


MinorMode::~MinorMode() {}


void MinorMode::start()
{
    LCD.clear();  // TODO: remove this crap
    LCD.print("Starting ABSmm...");
    delay(1000);

    this->active = true;
}


void MinorMode::stop()
{
    this->init();
    this->pause();
}


void MinorMode::pause()
{
    this->active = false;
}


void MinorMode::test()
{
    this->active = true;
}


bool MinorMode::isActive()
{
    return this->active;
}
