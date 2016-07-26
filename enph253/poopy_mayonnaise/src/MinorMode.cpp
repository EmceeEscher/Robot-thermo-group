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
