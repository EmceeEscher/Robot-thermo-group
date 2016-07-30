///
// IMode.hpp
//
// Mode interface class, to define general behaviors of major and minor
// modes
//
#ifndef I_MODE_HPP
#define I_MODE_HPP

class IMode
{

public:

    virtual ~IMode() {}

    /*
     * Major loop function for mode
     */
    virtual void loop() = 0;

    /*
     * Begin looping
     */
    virtual void start() = 0;

    /*
     * Stop looping (and reset variables)
     */
    virtual void stop() = 0;

    
    /*
     * Stop looping but keep variables in current state
     */
    virtual void pause() = 0;

    /*
     * Enter a testing mode, wherein active looping is engaged but hardware
     * (i.e. motors, etc) are not activated
     */
    virtual void test() = 0;

    /*
     * Return true if the mode is currently active
     */
    virtual bool isActive() = 0;
    
};

#endif  // I_MODE_HPP
