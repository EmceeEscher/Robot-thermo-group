///
// MToDestination.hpp
//
// Major mode for navigating to the dropoff location
//
#ifndef M_TO_DESTINATION_HPP
#define M_TO_DESTINATION_HPP

#include "MajorMode.hpp"


class MToDestination : public MajorMode
{

private:

    bool active;
    
    /*
     * (Re)initialize state variables
     */
    void init();

public:

    MToDestination();  // constructor

    ~MToDestination();  // deconstructor

    void loop();

    void start();

    void stop();

    void pause();

    void test();

    bool isActive();

};


#endif  // M_TO_DESTINATION_HPP
