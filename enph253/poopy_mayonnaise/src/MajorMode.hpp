///
// MajorMode.hpp
//
// Major mode (singular objective) controller interface class
//
#ifndef MAJOR_MODE_HPP
#define MAJOR_MODE_HPP

class MajorMode
{
public:
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
     * Return true if the mode is currently active
     */
    virtual bool isActive() = 0;
    
    /*
     * Enter next major mode
     */
    virtual void nextMode() = 0;
};

#endif // MAJOR_MODE_HPP
