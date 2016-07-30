///
// MinorMode.cpp
//
#include "MinorMode.hpp"


void MinorMode::init()
{
    this->active = false;
}


MinorMode::MinorMode()
    : active(false)
{
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
