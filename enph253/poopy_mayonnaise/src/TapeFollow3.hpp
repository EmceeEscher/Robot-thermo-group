///
// TapeFollow3.hpp
//
// A third try for a tape follower where distinct actions are separated
//
#ifndef TAPE_FOLLOW_3_HPP
#define TAPE_FOLLOW_3_HPP


#include "MinorMode.hpp"


class TapeFollow3 : MinorMode
{
private:
    const double gainProp;      // TODO: set const; set based on knobs for now
    const double gainDer1;      // TODO: set const; set based on knobs for now
    const double gainDer2;      // TODO: set const; set based on knobs for now
    const double errorSmall;    // one main on tape
    const double errorMedium;   // both mains off, one intersection on tape
    const double errorLarge;    // all QRDs off tape
    const double errorTurning;  // error to be applied during turning
    double lastError;           // last calculated error
    double errorArray[2];       // array of last 2 distinct errors
    unsigned long etimeArray[2];       // array of times (since read) assoc with errorArray
    int turnDirection;          // current direction (-1:left, 0:straight, 1:right)
    bool onTape;                // true= on tape, false= off tape
    bool lastOnTape;            // last value of onTape
    bool mainsOnTape;           // whether one of the mains in on the tape
    bool lastMainsOnTape;       // whether one of the mains was on the tape in the last step
    bool turning;               // true= turning, false= straight
    bool halfTurn;              // if true, bot has turned far enough that mains are off tape
    bool active;                // whether the loop is active
    int activePins[4];          // pin numbers (intL, mainL, mainR, intR)
    bool lastPinReadings[4];    // previous pin readings
    bool pinReadings[4];        // current readings on QRD pins
    int motorSpeed;             // speed to add to motors
    bool motorsActive;          // true if motors are active
    int control;                // current control parameter
    bool intersectSeen[2];      // true if an intersection was seen
    bool intersectDetect[2];    // true when an intersection has been detected (seen and passed over)
    const unsigned long intersectDelay; 
    unsigned long tapeFollowSteps;

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
    double seekTape();

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
    double followTape();

    /*
     * Function which chooses what direction to turn, based on its available
     * options
     * 
     * param left: true if left is an option, false otherwise
     * param right: true if right is an option, false otherwise
     * param straight: true if straight is an option, false otherwise
     */
    static int chooseTurn(bool left, bool right, bool straight);

    /*
     * Loop function for completing a turn in a single direction.
     * Continues until both main detecters loss current tape and find the
     * next, at which point `followTape` is entered.
     * The appropriate error is returned.
     */
    double makeTurn();

    /*
     * Print output to the LCD
     *
     * param control: the control for printing
     */
    void printLCD();

public:
    TapeFollow3();

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
     * Stop the loop and reset all variables.
     */
    void stop();

    /*
     * Stop the loop, but keep the current state of all variables.
     */
    void pause();

    /*
     * Return true if the mode is currently active, else false
     */
    bool isActive();

    /*
     * Enter a mode where readings are still made, but motors are always
     * at speed 0.
     */
    void test();
};

#endif // TAPE_FOLLOW_3_HPP
