///
// TapeFollow.hpp
//
// Minor mode for following tape
//
#ifndef TAPE_FOLLOW_HPP
#define TAPE_FOLLOW_HPP

#include <StandardCplusplus.h>
#include <bitset>
#include <vector>
#include "pins.hpp"
#include "Direction.hpp"
#include "MinorMode.hpp"

using std::vector;
using std::bitset;


enum class TFAction
{
    FOLLOWING,
    REVERSING,
    SEEKING,
    TURNING
};


class TapeFollow : public MinorMode
{

private:

    static const int motorPinL;
    static const int motorPinR;
    static const int *tapeSensorsFront;
    static const int *tapeSensorsBack;
    static const int numSensors = pins_sizes::TAPE_SENSORS_FRONT;
    static const int numActions = 4;
    static const TFAction allActions[TapeFollow::numActions];

    float gainProp;                        // TODO: set const; set based on knobs for now
    float gainDer1;                        // TODO: set const; set based on knobs for now
    float gainDer2;                        // TODO: set const; set based on knobs for now
    const float errorSmall;                // one main on tape
    const float errorMedium;               // both mains off, one intersection on tape
    const float errorLarge;                // all QRDs off tape
    const float errorSeeking;              // error to apply while seeking tape
    const float errorTurning;              // error to be applied during turning
    const int intersectSeekDelayPeriod;    // while tape following, waits for this many steps before searching for intersections
    const int intersectDetectPeriod;       // number of consecutive readings required to see an intersection
    const int turnConfirmPeriod;           // number of consecutive readings required to register start of turning
    const int preTurnDelayPeriod;          // number of iterations to wait after detecting intersections before making decision
    const int preTurnAroundDelayPeriod;    // number of reverse steps to make before turning around
    const int offTapePeriod;               // number of consecutive readings required to signal that the robot has lost the tape
    const int onTapePeriod;                // number of consecutive readings required to confirm that the robot is back on the tape after turning
    const int printPeriod;                 // number of iterations per printout
    const int counterMax;                  // maximum value for onTapeCounter and offTapeCounter
    const int motorSpeedTurningDefault;    // motor speed for making turn
    const int motorSpeedTurningAround;     // motor speed for turning around
    const int motorSpeedSeeking;           // motor speed for seeking tape
    const int motorSpeedFollowingDefault;  // default motor speed for tape following
    const int motorSpeedPassengerSeek;     // motor speed for following after initial passenger sighting
    const int motorSpeedReverse;           // motor speed for backing up
    int motorSpeedFollowing;               // current motor speed for following tape
    int motorSpeedTurning;

    bool turningAround;           // true if the robot is turning around
    bool willTurnAround;          // true if the robot is about to turn around
    bool halfTurn;                // if true, bot has turned far enough that mains are off tape
    bool motorsActive;            // true if motors are active

    TFAction action;              // what the tapefollowing is currently doing (i.e. following, seeking, etc)

    Direction turnDirection;      // current turn direction
    float leftWeight;             // TODO: come up with a better way of doing this weight stuff
    float straightWeight;
    float rightWeight;
    
    int control;                  // current control parameter
    int printCount;
    int motorSpeed;               // speed to add to motors
    float error;
    float lastError;              // last calculated error

    bitset<numSensors> pinReadings;    // current readings on QRD pins
    bitset<2> intersectSeen;            // true if an intersection was seen
    bitset<2> intersectDetect;          // true when an intersection has been detected (seen and passed over)

    vector<float> errorArray;           // array of last 2 distinct errors
    vector<unsigned long> etimeArray;   // array of times (since read) assoc with errorArray
    int activePins[numSensors];         // pin numbers (intL, mainL, mainR, intR)

    int steps[TapeFollow::numActions];  // number of steps for associated action

    int onTapeCounter[numSensors];      // counts the number of consecutive onTape reads for each pin
    int offTapeCounter[numSensors];     // counts the number of consecutive offTape reads for each pin
    
    /*
     * Set all instance variables to their default starting values
     */
    void init();

    /*
     * Returns true if off tape (no readings) for longer than offTapePeriod
     */
    bool offTape();

    /*
     * Returns true if both mains are off tape for longer than offTapePeriod
     */
    bool mainsOffTape();

    /*
     * Based on the lastPinReadings array, determines whether an
     * intersection has been seen and updates the intersectSeen
     * array
     */
    void updateIntersectionsSeen();

    /*
     * Look for intersection. If found, make decision and turn?
     */
    void updateIntersectionsDetected();

    /*
     * Function which chooses what direction to turn, based on its available
     * options
     * 
     * Chooses randomly, but weighted by values giveTurnDirection.
     * If giveTurnDirection hasn't been called since last call of chooseTurn,
     * will choose randomly with equal weights given to available options.
     * 
     * param left: true if left is an option, false otherwise
     * param right: true if right is an option, false otherwise
     * param straight: true if straight is an option, false otherwise
     */
    Direction chooseTurn(bool left, bool right, bool straight);

    /*
     * Chooses direction to turn but not randomly. It will always turn in the highest
     * weighted direction if it can, next highest if it can't, etc.
     * 
     * If all weights are equal, behavior is undefined
     */
    Direction chooseTurnDeterministic(bool left, bool right, bool straight);

    /*
     * Loop function for seeking tape. 
     * Intersection sensors are used to help find tape. When found,
     * `followTape` is entered.
     * The appropriate error is set.
     */
    void setErrorSeekTape();

    /*
     * Loop function for following tape with no intersections.
     * If an intersection is detected (and a turn is to be made) 
     * `makeTurn` is called.
     * The appropriate error is set.
     */
    void setErrorFollowTape();

    /*
     * Loop function for completing a turn in a single direction.
     * Continues until both main detecters loss current tape and find the
     * next, at which point `followTape` is entered.
     * The appropriate error is returned.
     */
    void setErrorMakeTurn();

    /*
     * Based on whether the robot is currently seeking, turning, or following,
     * set the error to apply.
     */
    void setError();

    /*
     * Based on the error and following speed, set the control to apply to
     * the motor speed
     */
    void setControl();

    /*
     * Update on/off tape counters
     */
    void updateCounters();

    /*
     * Updates the state of the robot based on the latest reads and
     * counter values
     */
    void updateState();

    /*
     * Print output to the LCD
     *
     * param control: the control for printing
     */
    void printLCD();

public:

    TapeFollow();   // constructor

    ~TapeFollow();  // deconstructor

    /*
     * Main loop funciton to follow tape. Uses `seekTape`, `followTape`,
     * and `makeTurn` to control behavior.
     */
    void loop();

    /*
     * Begin the loop
     */
    void start();

    /*
     * Stop the loop, but keep the current state of all variables.
     */
    void pause();

    /*
     * Enter a mode where readings are still made, but motors are always
     * at speed 0.
     */
    void test();

    /*
     * Changes the motor following speed to the given value
     */
    void setMotorSpeedPassengerSeek();  // TODO: find a better way to do this

    /*
     * Resets the following speed to the default value
     */
    void resetMotorSpeed();  // TODO: find a better way to do this

    /*
     * Turn around procedure
     */
    void turnAround();

    /*
     * Return true if the robot is currently turning; else false
     */
    bool isTurning();

    /*
     * Return true if the robot is currently seeking tape
     */
    bool isSeeking();

    /*
     * Give weighted directions for turning. Robo will turn at next intersection randomly,
     * but weighted by ratio of parameters. If you want it to only turn in one direction, 
     * give it 0 for the others. If it can't turn in a nonzero direction, it will turn randomly
     * in one of the directions it can go.
     */
    void giveTurnDirection(float left, float right, float straight);

};

#endif // TAPE_FOLLOW_HPP
