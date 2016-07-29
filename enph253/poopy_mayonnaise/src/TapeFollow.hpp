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
#include "Direction.hpp"
#include "MinorMode.hpp"

using std::vector;
using std::bitset;
using readingFn_t = bool(*)(vector<bool>);

class TapeFollow : public MinorMode
{

private:

    static const int motorPinL;
    static const int motorPinR;
    static const int *tapeSensorsFront;
    static const int *tapeSensorsBack;
    static const int numSensors;

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
    const int turnPreDelayPeriod;          // number of iterations to wait after detecting intersections before making decision
    const int offTapePeriod;               // number of consecutive readings required to signal that the robot has lost the tape
    const int onTapePeriod;                // number of consecutive readings required to confirm that the robot is back on the tape after turning
    const int printPeriod;                 // number of iterations per printout
    const int counterMax;                  // maximum value for onTapeCounter and offTapeCounter
    const int motorSpeedTurningDefault;    // motor speed for making turn
    const int motorSpeedSeeking;           // motor speed for seeking tape
    const int motorSpeedFollowingDefault;  // default motor speed for tape following
    const int motorSpeedPassengerSeek;     // motor speed for following after initial passenger sighting
    const int motorSpeedReverse;           // motor speed for backing up
    int motorSpeedFollowing;               // current motor speed for following tape
    int motorSpeedTurning;

    bool onTape;                  // true= on tape, false= off tape
    bool lastOnTape;              // last value of onTape
    bool mainsOnTape;             // whether one of the mains in on the tape
    bool lastMainsOnTape;         // whether one of the mains was on the tape in the last step
    bool seeking;                 // whether the robot is currently seeking tape
    bool turning;                 // true= turning, false= straight
    bool turningAround;           // true if the robot is turning around (this->turning will always be true if this is true)
    bool halfTurn;                // if true, bot has turned far enough that mains are off tape
    bool motorsActive;            // true if motors are active

    Direction turnDirection;      // current turn direction
    int control;                  // current control parameter
    int printCount;
    int motorSpeed;               // speed to add to motors
    int tapeFollowSteps;
    float lastError;              // last calculated error

    bitset<4> pinReadings;        // current readings on QRD pins
    bitset<2> intersectSeen;      // true if an intersection was seen
    bitset<2> intersectDetect;    // true when an intersection has been detected (seen and passed over)

    vector<float> errorArray;           // array of last 2 distinct errors
    vector<unsigned long> etimeArray;   // array of times (since read) assoc with errorArray
    int activePins[4];                  // pin numbers (intL, mainL, mainR, intR)

    int onTapeCounter[4];               // counts the number of consecutive onTape reads for each pin
    int offTapeCounter[4];              // counts the number of consecutive offTape reads for each pin
    
    /*
     * Set all instance variables to their default starting values
     */
    void init();

    /*
     * Loop function for seeking tape. 
     * Intersection sensors are used to help find tape. When found,
     * `followTape` is entered.
     * The appropriate error is returned.
     */
    float seekTape();

    /*
     * Based on the lastPinReadings array, determines whether an
     * intersection has been seen and updates the intersectSeen
     * array
     */
    void intersectionSeen();

    /*
     * Look for intersection. If found, make decision and turn?
     */
    void intersectionDetection();

    /*
     * Loop function for following tape with no intersections.
     * If an intersection is detected (and a turn is to be made) 
     * `makeTurn` is called.
     * The appropriate error is returned.
     */
    float followTape();

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
    static Direction chooseTurn(bool left, bool right, bool straight);

    /*
     * Returns true if readingFn is true for all of lastPinReadings in
     * [0, period)
     *
     * param period: number of readings back to look at
     * param fn: reading->bool function to test on readings
     */
    bool fnAllLastReadings(int period, readingFn_t fn);

    /*
     * Returns true if readingFn is true for any of lastPinReadings in
     * [0, period)
     *
     * param period: number of readings back to look at
     * param fn: reading->bool function to test on readings
     */
    bool fnAnyLastReadings(int period, readingFn_t fn);
    
    /*
     * Loop function for completing a turn in a single direction.
     * Continues until both main detecters loss current tape and find the
     * next, at which point `followTape` is entered.
     * The appropriate error is returned.
     */
    float makeTurn();

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
    void setMotorSpeedPassengerSeek();

    /*
     * Resets the following speed to the default value
     */
    void resetMotorSpeed();

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
