///
// MajorMode.hpp
//
// Major mode (singular objective) controller abstract class.
// Method implemenations contain the default action to be done for any
// major mode.
//
#ifndef MAJOR_MODE_HPP
#define MAJOR_MODE_HPP

#include <StandardCplusplus.h>
#include <vector>
#include "IMode.hpp"
#include "MinorMode.hpp"

using std::vector;

class MajorMode : public IMode
{

protected:

    bool active;

    vector< MinorMode* > allMinorModes;

    MajorMode()
	: active(false)
    {
	this->init();
    }

    virtual void init()
    {
	this->active = false;
    }
    
public:

    virtual ~MajorMode()
    {
	for (auto *mm : this->allMinorModes)
	    delete mm;
    }

    virtual void loop()
    {
	for (auto *mm : this->allMinorModes)
	    if (mm->isActive())
		mm->loop();
    }

    virtual void start()
    {
	this->active = true;
    }

    virtual void stop()
    {
	this->init();
	this->pause();
    }

    virtual void pause()
    {
	this->active = false;
    }

    virtual void test()
    {
	this->active = true;
    }

    virtual bool isActive()
    {
	return active;
    }

};

#endif // MAJOR_MODE_HPP
