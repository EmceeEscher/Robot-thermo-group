///
// ArmControl.hpp
//
// Minor mode for picking up/dropping passengers

#ifndef ARM_CONTROL_HPP
#define ARM_CONTROL_HPP

#include "MinorMode.hpp"

class ArmControl : public MinorMode
{

private:

    //Analog pin for base angle potentiometer signal
    static const int baseAnglePin;
    //Base GBBC motor output number
    static const int baseMotorNumber;
    //Claw's Baby Motor output number
    static const int babyMotorNum;
    //Digital pin indicating a successfully caugh animal
    static const int catchSwitch;
    //Digital pin indicating a claw closing on itself
    static const int noCatchSwitch;
    //Digital pin to detect when an animal is in the claw
    static const int innerClawSwitch;

    //Stepper Constants
    static const int stepperDirPin;
    static const int stepperPulsePin;

    //Amount of time to run motor to drop animal
    const int dropTime; // Milliseconds

    //SPECIFIED GAIN VALUES
    const float propGain;
    const float holdPropGain;
    const float intGain;
    const float derivGain;

    //Rest Positions
    const float baseRestPosition;
    const float midRestPosition;
    const float baseHoldPosition;
    const float midHoldPosition;

    const int stepperMicrosDelay; //Time delay between pulses in microseconds
    const int numPulses;

    //reachAndClaw/reachAndDrop function Constants
    const float initialAdjMidTarget;
    const float initialAdjBaseTarget;
    const float finalAdjMidTarget;
    const float finalAdjBaseTarget;
    const float midAdjMidTarget;
    const float midAdjBaseTarget;

    //Iterator for LCD printing
    int lcdControl;  // TODO: how is an int an interator??

    //Dynamic Control Variables
    /*float baseTarget, midTarget;
    float propErr, derivErr, intErr, lastPropErr;
    float angle;
    float currAngle;
    float now, lastTime;
    int hasInitialized;*/
    float baseTarget;
    float midTarget;
    float propErr;
    float derivErr;
    float intErr;
    float lastPropErr;
    float angle;
    float now;
    float lastTime;

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
    
    /*
    //Converts base potentiometer voltage to corresponding angle
    */
    float getAngle();
    
    //Wrapper function for setting motor speed
    //Prevents values larger than 255 in either direction
    void setBaseMotor(int duty);
    
    //Returns the motor speed based on PID control
    float getControlValue();
    
    //Closes the claw until something is detected in claw, the claw
    //closes on itself or a timeout is reached
    void grabCrap();
    
    //Opens the claw for specified time
    void dropCrap();
    
    //Extends arm over two periods and either grabs or drops
    void reachAndClaw(bool grabbing);
    
    //Sets the control target values to rest position
    void setRestPosition();
    
    //Turns the stepper motor a specified number of steps
    void stepperTurn(bool CW, int count);
    
    /*
     * Parameter: turnRight - Turn right if true, left otherwise
     * Parameter: grab - grab if true, drop otherwise
     */
    void turnAndReach(bool turnRight, bool grab);
    
    /*
     * Returns true if the robot is currently holding a doll,
     * false otherwise
     */
    bool isHolding();
    
    //prints angle read by potentiometer for testing
    void printState();
};


#endif //ARM_CONTROL_HPP
