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

    /*
     * (Re)initialize state variables
     */
    void init();

public:

    MToDestination();  // constructor

    ~MToDestination();  // deconstructor

    /*
     * Major loop function for handling associated minor modes
     */
    void loop();

    /*
     * Enter a testing mode, in which hardware is not active
     */
    void test();

};


#endif  // M_TO_DESTINATION_HPP
