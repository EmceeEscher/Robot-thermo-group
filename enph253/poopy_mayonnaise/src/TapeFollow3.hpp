///
// TapeFollow3.hpp
//
// A third try for a tape follower where distinct actions are separated
//
#ifndef TAPE_FOLLOW_3_HPP
#define TAPE_FOLLOW_3_HPP


#include <phys253.h>


class TapeFollow3 {
private:
    double gainProp;           // set based on knobs for now
    double gainDer1;           // set based on knobs for now
    double gainDer2;           // set based on knobs for now
    double lastError;          // last calculated error
    double errorArray[2];      // array of last 2 distinct errors
    long etimeArray[2];        // array of times (since read) assoc with errorArray
    int turnDirection;         // current direction (-1:left, 0:straight, 1:right)
    bool onTape;               // true= on tape, false= off tape
    bool turning;              // true= turning, false= straight
    float active;              // whether the loop is active
    int activePins[4];         // pin numbers (intL, mainL, mainR, intR)
    int motorSpeed;            // speed to add to motors
    bool motorsActive;         // true if motors are active

    /*
     * Set all instance variables to their default starting values
     */
    void init();

    /*
     * Loop function for seeking tape. 
     * Intersection sensors are used to help find tape. When found,
     * `followTape` is entered.
     */
    void seekTape();

    /*
     * Loop function for following tape with no intersections.
     * If an intersection is detected (and a turn is to be made) 
     * `makeTurn` is called.
     */
    void followTape();

    /*
     * Function which chooses what direction to turn, based on its available
     * options
     * 
     * param left: true if left is an option, false otherwise
     * param right: true if right is an option, false otherwise
     * param straight: true if straight is an option, false otherwise
     */
    int chooseTurn(bool left, bool right, bool straight);

    /*
     * Loop function for completing a turn in a single direction.
     * Continues until both main detecters loss current tape and find the
     * next, at which point `followTape` is entered.
     */
    void makeTurn();

    /*
     * Print output to the LCD
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
     * Enter a mode where readings are still made, but motors are always
     * at speed 0.
     */
    void test();
};

#endif // TAPE_FOLLOW_3_HPP
