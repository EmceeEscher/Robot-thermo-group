///
// MinorMode.hpp
//
// Minor mode (sub-objective to be run concurrently with a MajorMode)
// interface class
//
#ifndef MINOR_MODE_HPP
#define MINOR_MODE_HPP

class MinorMode {
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
};

#endif // MINOR_MODE_HPP
