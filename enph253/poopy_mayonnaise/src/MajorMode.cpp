///
// MajorMode.cpp
//
#include "MajorMode.hpp"


void MajorMode::init()
{
    this->active = false;
}


MajorMode::MajorMode()
    : active(false)
{
    this->init();
}


MajorMode::~MajorMode()
{
    for (auto *mm : this->allMinorModes)
	delete mm;
}


void MajorMode::loop()
{
    for (auto *mm : this->allMinorModes)
	if (mm->isActive())
	    mm->loop();
}


void MajorMode::start()
{
    this->active = true;
}


void MajorMode::stop()
{
    this->init();
    this->pause();
}


void MajorMode::pause()
{
    this->active = false;
}


void MajorMode::test()
{
    this->active = true;
}


bool MajorMode::isActive()
{
    return this->active;
}
