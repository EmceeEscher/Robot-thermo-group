///
// Arm_And_Stepper.hpp
//
#ifndef ARM_AND_STEPPER_HPP
#define ARM_AND_STEPPER_HPP

namespace Arm_And_Stepper
{

    void setup();

    void doControl();

    void turnAndReach(bool turnRight, bool grab);

    void dropCrap();

}

#endif // ARM_AND_STEPPER_HPP
