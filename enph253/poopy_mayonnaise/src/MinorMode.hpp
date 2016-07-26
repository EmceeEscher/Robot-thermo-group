///
// MinorMode.hpp
//
// Minor mode (sub-objective to be run concurrently with a MajorMode)
// abstract class
//
#ifndef MINOR_MODE_HPP
#define MINOR_MODE_HPP

#include "IMode.hpp"

class MinorMode : public IMode
{

protected:

    bool active;

    /*
     * Default `init()` method for a MinorMode. Deactivates the mode.
     */
    virtual void init();

    /*
     * Default constructor for a MinorMode. Initializes active to false
     */
    MinorMode();

public:

    virtual ~MinorMode();

    /*
     * Default `start()` method for a MinorMode. Activates the mode.
     */
    virtual void start();

    /*
     * Default `stop()` method for a MinorMode. Resets the state variables
     * and pauses the mode.
     */
    virtual void stop();

    /*
     * Default `pause()` method for a MinorMode. Deactivates the mode but
     * maintains the current state.
     */
    virtual void pause();

    /*
     * Default `test()` method for a MinorMode. Activates the mode.
     */
    virtual void test();

    /*
     * Return true if the MinorMode is active
     */
    virtual bool isActive();
    
};

#endif // MINOR_MODE_HPP
