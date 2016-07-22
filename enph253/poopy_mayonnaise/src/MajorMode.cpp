///
// MajorMode.cpp
//
#include <phys253.h>
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
    LCD.clear();  // TODO: remove this crap
    LCD.print("ABSmLOOPY");
    LCD.setCursor(0,1);
    LCD.print(this->allMinorModes.size());
    delay(1000);

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
