///
// CollisionWatch.cpp
//
#include "CollisionWatch.hpp"


// TODO
void CollisionWatch::init()
{
    this->active = false;
}


// TODO
CollisionWatch::CollisionWatch()
    : active(false)
{
    this->init();
}


CollisionWatch::~CollisionWatch() {}


// TODO
void CollisionWatch::loop()
{
}


// TODO
void CollisionWatch::start()
{
    this->active = true;
}


void CollisionWatch::stop()
{
    this->init();
    this->pause();
}


// TODO
void CollisionWatch::pause()
{
    this->active = false;
}


// TODO
void CollisionWatch::test()
{
    this->active = true;
}


bool CollisionWatch::isActive()
{
    return this->active;
}
