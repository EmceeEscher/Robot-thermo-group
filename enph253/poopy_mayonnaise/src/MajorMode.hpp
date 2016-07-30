///
// MajorMode.hpp
//
// Major mode (singular objective) controller abstract class.
// Method implemenations contain the default action to be done for any
// major mode.
//
#ifndef MAJOR_MODE_HPP
#define MAJOR_MODE_HPP

#include "IMode.hpp"
#include "MinorMode.hpp"
#include "MajorModeEnum.hpp"


class MajorMode : public IMode
{

protected:

    const int numMinorModes;
    MinorMode **allMinorModes;  // Pointer to an array of pointers... Welcome to C++

    const MajorModeEnum thisMode;
    MajorModeEnum nextMode;

    bool active;

    // vector< MinorMode* > allMinorModes;

    /*
     * Default major mode `init` method. Sets active to false.
     */
    virtual void init();

    /*
     * Default major mode initializer. Sets active to false.
     */
    MajorMode(MajorModeEnum thisMode, int numMinorModes);
    
public:

    /*
     * Deallocates all MinorMode objects pointed to by `allMinorModes`
     */
    virtual ~MajorMode();

    /*
     * Default MajorMode `loop` function, which calls the `loop` method
     * of each active minor mode in `allMinorModes`
     */
    virtual void loop();

    /*
     * Default `start` for MajorMode. Sets `active` to true.
     */
    virtual void start();

    /*
     * Default `stop` method for a MajorMode. Calls the most derived `init`
     * and then the most derived `pause`.
     */
    virtual void stop();

    /*
     * Default `pause` method for a MinorMode. Sets `active` to false.
     */
    virtual void pause();

    /*
     * Default `test` method for a MajorMode. Sets `active` to true.
     */
    virtual void test();

    /*
     * Returns true if the MajorMode is active.
     */
    virtual bool isActive();

    /*
     * Returns a member of MajorModeEnum telling you which mode RobotState
     * should switch to, or NoChange if there is not to be a change
     */
     virtual MajorModeEnum changeTo();

};

#endif // MAJOR_MODE_HPP
