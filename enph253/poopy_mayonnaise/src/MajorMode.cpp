///
// MajorMode.cpp
//
#include "MajorMode.hpp"


void MajorMode::init()
{
    this->active = false;
}


MajorMode::MajorMode() : active(false) {}


MajorMode::~MajorMode() {}


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
    // stop all minor modes
    for (auto *mm : this->allMinorModes)
	mm->stop();
}


void MajorMode::pause()
{
    this->active = false;
    // pause all minor modes
    for (auto *mm : this->allMinorModes)
	mm->pause();
}


void MajorMode::test()
{
    this->active = true;
}


bool MajorMode::isActive()
{
    return this->active;
}
