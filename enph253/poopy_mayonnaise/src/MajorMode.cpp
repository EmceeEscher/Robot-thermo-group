///
// MajorMode.cpp
//
#include "MajorModeEnum.hpp"
#include "MajorMode.hpp"


void MajorMode::init()
{
    this->active = false;
    this->nextMode = this->thisMode;
}


MajorMode::MajorMode(MajorModeEnum thisMode, int numMinorModes)
    : numMinorModes(numMinorModes),
      thisMode(thisMode),
      nextMode(thisMode),
      active(false)
{}


MajorMode::~MajorMode()
{
    // deleting individual minor modes is the responsibility of the owner

    // delete allMinorModes array
    delete[] this->allMinorModes;
}


void MajorMode::loop()
{
    // for (auto *mm : this->allMinorModes)
    // 	if (mm->isActive())
    // 	    mm->loop();
    for (int i(0); i < this->numMinorModes; ++i) {
	MinorMode *mm = this->allMinorModes[i];
	if (mm->isActive())
	    mm->loop();
    }
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
    // for (auto *mm : this->allMinorModes)
    // 	mm->stop();
    for (int i(0); i < this->numMinorModes; ++i) 
	this->allMinorModes[i]->stop();
}


void MajorMode::pause()
{
    this->active = false;
    // pause all minor modes
    // for (auto *mm : this->allMinorModes)
    // 	mm->pause();
    for (int i(0); i < this->numMinorModes; ++i)
	this->allMinorModes[i]->pause();
}


void MajorMode::test()
{
    this->pause();
    this->active = true;
}


bool MajorMode::isActive()
{
    return this->active;
}


MajorModeEnum MajorMode::changeTo()
{
    return this->nextMode;
}

