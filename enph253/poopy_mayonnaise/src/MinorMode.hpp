///
// MinorMode.hpp
//
// Minor mode (sub-objective to be run concurrently with a MajorMode)
// interface class
//
#ifndef MINOR_MODE_HPP
#define MINOR_MODE_HPP

#include "IMode.hpp"

class MinorMode : public IMode
{

protected:

    bool active;

    MinorMode()
	: active(false)
    {
	this->init();
    }
    
    virtual void init()
    {
	this->active = false;
    }

public:

    virtual ~MinorMode() {};

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

#endif // MINOR_MODE_HPP
