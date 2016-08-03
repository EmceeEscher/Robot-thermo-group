///
// Arm_And_Stepper.hpp
//
#ifndef ARM_AND_STEPPER_HPP
#define ARM_AND_STEPPER_HPP


namespace Arm_And_Stepper
{

    extern const int dropTime;
    extern bool holding;

    void setup();
    void doControl();
    float getAngle();
    void dropCrap();
    void stepperTurn(bool CW,int count);
    void turnAndReach(bool turnRight, bool grab);

}


#endif // ARM_AND_STEPPER_HPP
