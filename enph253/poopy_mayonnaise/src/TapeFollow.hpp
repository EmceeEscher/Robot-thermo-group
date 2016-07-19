///
// TapeFollow.hpp
//
// A third try for a tape follower where distinct actions are separated
//
#ifndef TAPE_FOLLOW_HPP
#define TAPE_FOLLOW_HPP


#include <StandardCplusplus.h>
#include <vector>
#include "MinorMode.hpp"


using std::vector;
using readingFn_t = bool(*)(vector<bool>);


class TapeFollow : public MinorMode
{

private:

    double gainProp;              // TODO: set const; set based on knobs for now
    double gainDer1;              // TODO: set const; set based on knobs for now
    double gainDer2;              // TODO: set const; set based on knobs for now
    const double errorSmall;      // one main on tape
    const double errorMedium;     // both mains off, one intersection on tape
    const double errorLarge;      // all QRDs off tape
    const double errorSeeking;    // error to apply while seeking tape
    const double errorTurning;    // error to be applied during turning
    const unsigned long intersectDelay;  // while tape following, waits for this many steps before searching for intersections
    const int intersectPeriod;       // number of consecutive readings required to see an intersection
    const int turningPeriod;         // number of consecutive readings required to register start of turning
    const int turnWaitPeriod;        // number of iterations to wait after detecting intersections before making decision
    const int offTapePeriod;         // number of consecutive readings required to signal that the robot has lost the tape
    const int onTapePeriod;          // number of consecutive readings required to confirm that the robot is back on the tape after turning
    const int printPeriod;           // number of iterations per printout
    const int motorSpeedFollowing;   // motor speed for following tape
    const int motorSpeedTurning;     // motor speed for making turn

    bool active;                  // whether the loop is active
    bool onTape;                  // true= on tape, false= off tape
    bool lastOnTape;              // last value of onTape
    bool mainsOnTape;             // whether one of the mains in on the tape
    bool lastMainsOnTape;         // whether one of the mains was on the tape in the last step
    bool turning;                 // true= turning, false= straight
    bool halfTurn;                // if true, bot has turned far enough that mains are off tape
    bool motorsActive;            // true if motors are active

    int turnDirection;            // current direction (-1:left, 0:straight, 1:right)
    int control;                  // current control parameter
    int printCount;
    int motorSpeed;               // speed to add to motors
    unsigned long tapeFollowSteps;
    double lastError;             // last calculated error

    vector<bool> pinReadings;                // current readings on QRD pins
    vector< vector<bool> > lastPinReadings;  // array of previous time readings
    vector<bool> intersectSeen;              // true if an intersection was seen
    vector<bool> intersectDetect;            // true when an intersection has been detected (seen and passed over)
    vector<double> errorArray;               // array of last 2 distinct errors
    vector<unsigned long> etimeArray;        // array of times (since read) assoc with errorArray
    vector<int> activePins;                  // pin numbers (intL, mainL, mainR, intR)
    
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
     * Returns true if readingFn is true for all of lastPinReadings in
     * [0, period)
     */
    bool fnAllLastReadings(int period, readingFn_t fn);

    /*
     * Returns true if readingFn is true for any of lastPinReadings in
     * [0, period)
     */
    bool fnAnyLastReadings(int period, readingFn_t fn);
    
    /*
     * Returns true if the given reading is all false
     */
    static bool offTape(vector<bool> reading);

    /*
     * Returns true if both of the mains are off of the tape
     */
    static bool mainsOffTape(vector<bool> reading);

    /*
     * Return true if both of the intersections are off of the tape
     */
    static bool intsOffTape(vector<bool> reading);

    /*
     * Return true if the left intersection is on the tape
     */
    static bool intLOnTape(vector<bool> reading);

    /*
     * Return true if the right intersection is on the tape
     */
    static bool intROnTape(vector<bool> reading);
    
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

#endif // TAPE_FOLLOW_HPP
