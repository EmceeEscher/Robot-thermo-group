///
// PassengerSeek.cpp
//
#include "PassengerSeek.hpp"


// TODO
void PassengerSeek::init()
{
    this->active = false;
}


// TODO
PassengerSeek::PassengerSeek()
    : active(false)
{
    this->init();
}


PassengerSeek::~PassengerSeek() {}


// TODO
void PassengerSeek::loop()
{
    // TODO: Update QSD readings array
    // TODO: Determine derivatives
    // TODO: If at a maximum, stop tape following
}


// TODO
void PassengerSeek::start()
{
    this->active = true;
}


void PassengerSeek::stop()
{
    this->init();
    this->pause();
}


// TODO
void PassengerSeek::pause()
{
    this->active = false;
}


// TODO
void PassengerSeek::test()
{
    this->active = true;
}


bool PassengerSeek::isActive()
{
    return this->active;
}
