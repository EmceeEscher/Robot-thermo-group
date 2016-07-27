///
// ArmControl.hpp
//
// Minor mode for picking up/dropping passengers

#ifndef ARM_CONTROL_HPP
#define ARM_CONTROL_HPP

#include <StandardCplusplus.h>
#include "MinorMode.hpp"

class ArmControl : public MinorMode
{

private:

    //Analog pin for base angle potentiometer signal
    const int baseAnglePin;
    //Base GBBC motor output number
    const int baseMotorNumber;
 
    //Claw's Baby Motor output number
    const int babyMotorNum;
    //Digital pin indicating a successfully caugh animal
    const int catchSwitch;
    //Digital pin indicating a claw closing on itself
    const int noCatchSwitch;
    //Digital pin to detect when an animal is in the claw
    const int innerClawSwitch;
    //Amount of time to run motor to drop animal
    const int dropTime; // Milliseconds

    //SPECIFIED GAIN VALUES
    const float propGain;
    const float holdPropGain;
    const float intGain;
    const float derivGain;

    //Dynamic Control Variables
    float baseTarget, midTarget;
    float propErr, derivErr, intErr, lastPropErr;
    float angle;
    float currAngle;
    float now, lastTime;
    int hasInitialized;

    //Rest Positions
    const float baseRestPosition;
    const float midRestPosition;
    const float baseHoldPosition;
    const float midHoldPosition;

    //Iterator for LCD printing
    int LCDControl;

    //Stepper Constants
    const int stepperDirPin;
    const int stepperPulsePin;
    const int COUNTERCLOCKWISE;
    const int CLOCKWISE;
    const int stepperMicrosDelay; //Time delay between pulses in microseconds
    const int numPulses;

    //reachAndGrab/reachAndDrop function Constants
    const float initialAdjMidTarget;
    const float initialAdjBaseTarget;
    const float finalAdjMidTarget;
    const float finalAdjBaseTarget;

    //Holding a passenger?
    bool holding;
    
    
    //TODO: make it work
    void init();

public:
    
    
    //TODO: all
    ArmControl(); //constructor

    ~ArmControl(); //deconstructor
    
    /*
     * Main loop function, holds arm steady when not doing anything,
     * causes arm to turn and grab when it receives a message to do so
     */
    void loop();
    
    /*
     * PID control for the arm. Needs to be called very frequently to
     * ensure that the arm is held in the correct position
     */
    void doControl();
    
    
}	


#endif //ARM_CONTROL_HPP
