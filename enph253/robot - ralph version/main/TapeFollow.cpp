///
// TapeFollow.cpp
//
#include <StandardCplusplus.h>
#include <vector>
#include <bitset>
#include <phys253.h>
#include "pins.hpp"
#include "Direction.hpp"
#include "TapeFollow.hpp"


using std::vector;
using std::bitset;


namespace TapeFollow
{

    const int motorPinL         { pins::MOTOR_PIN_L };
    const int motorPinR         { pins::MOTOR_PIN_R };
    const int *tapeSensorsFront { pins::TAPE_SENSORS_FRONT };
    const int *tapeSensorsBack  { pins::TAPE_SENSORS_BACK };
    const int numSensors        { pins_sizes::TAPE_SENSORS_FRONT };
    const int numActions        {4};
    const TFAction allActions[numActions] = {
        TFAction::FOLLOWING,
        TFAction::REVERSING,
        TFAction::SEEKING,
        TFAction::TURNING
    };

    // Static function forward declarations

    /*
     * Returns true if off tape (no readings) for longer than OFF_TAPE_PERIOD
     */
    static bool offTape();

    /*
     * Returns true if both mains are off tape for longer than OFF_TAPE_PERIOD
     */
    static bool mainsOffTape();

    /*
     * Based on the lastPinReadings array, determines whether an
     * intersection has been seen and updates the intersectSeen
     * array
     */
    static void updateIntersectionsSeen();

    /*
     * Look for intersection. If found, make decision and turn?
     */
    static void updateIntersectionsDetected();

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
     * Chooses direction to turn but not randomly. It will always turn in the highest
     * weighted direction if it can, next highest if it can't, etc.
     * 
     * If all weights are equal, behavior is undefined
     */
    static Direction chooseTurnDeterministic(bool left, bool right, bool straight);

    /*
     * Set the error for following
     */
    static void setErrorFollowing();

    /*
     * Set the error for reversing
     */
    static void setErrorReversing();

    /*
     * Set the error for seeking
     */
    static void setErrorSeeking();

    /*
     * Set the error for turning
     */
    static void setErrorTurning();

    /*
     * Based on whether the robot is currently seeking, turning, or following,
     * set the error to apply.
     */
    static void setError();

    /*
     * Based on the error and following speed, set the control to apply to
     * the motor speed
     */
    static void setControl();

    /*
     * Update on/off tape counters
     */
    static void updateCounters();

    /*
     * Update the state and `action` of the robot, given that it is 
     * currenty following
     */
    static void updateStateFollowing();

    /*
     * Update the state and `action` of the robot, given that it is 
     * currenty reversing
     */
    static void updateStateReversing();

    /*
     * Update the state and `action` of the robot, given that it is 
     * currenty seeking
     */
    static void updateStateSeeking();

    /*
     * Update the state and `action` of the robot, given that it is 
     * currenty turning
     */
    static void updateStateTurning();

    /*
     * Updates the state of the robot based on the latest reads and
     * counter values
     */
    static void updateState();

}


const unsigned long RANDOM_MAX_VAL     {100000};

// Gains
const float EPSILON         {.10};
const float GAIN_PROP      {9.84};
const float GAIN_DER1     {11.78};
const float GAIN_DER2     {
        static_cast<const float>(
                .5 * GAIN_DER1 * GAIN_DER1 / GAIN_PROP * (1. - EPSILON))
};

// Errors
const float ERROR_SMALL     {.02};   // one main on tape
const float ERROR_MEDIUM    {.04};   // both mains off, one intersection on tape
const float ERROR_LARGE     {.08};   // all QRDs off tape
const float ERROR_SEEKING   {.64};   // error to apply while seeking tape
const float ERROR_TURNING {12.80};   // error to be applied during turning

// Delays
const int INTERSECT_SEEK_DELAY_PERIOD      {100};  // while tape following, waits for this many steps before searching for intersections
const int INTERSECT_DETECT_PERIOD           {15};  // number of consecutive readings required to see an intersection
const int TURN_CONFIRM_PERIOD               {10};  // number of consecutive readings required to register start of turning
const int PRE_TURN_DELAY_PERIOD             {75};  // number of iterations to wait after detecting intersections before making decision
const int PRE_TURN_AROUND_DELAY_PERIOD     {145};  // number of reverse steps to make before turning around
const int OFF_TAPE_PERIOD                   {50};  // number of consecutive readings required to signal that the robot has lost the tape
const int ON_TAPE_PERIOD                     {5};  // number of consecutive readings required to confirm that the robot is back on the tape after turning
const long TURN_AROUND_SPEED_SWITCH_PERIOD {2048};  // number of steps before switching between forward and reverse while turning around
const long TURN_AROUND_TIMEOUT_PERIOD      {8192};
const long COUNTER_MAX                    {16384}; // maximum value for onTapeCounter and offTapeCounter

// Speeds
const int MOTOR_SPEED_FOLLOWING       {84};  // default motor speed for tape following
const int MOTOR_SPEED_REVERSING      {-64};  // default motor speed for backing up
const int MOTOR_SPEED_SEEKING         {32};  // default motor speed for seeking tape
const int MOTOR_SPEED_TURNING         {32};  // default motor speed for making turn
const int DEFAULT_MOTOR_SPEEDS[TapeFollow::numActions] {
        MOTOR_SPEED_FOLLOWING,
        MOTOR_SPEED_REVERSING,
        MOTOR_SPEED_SEEKING,
        MOTOR_SPEED_TURNING
};
const int MOTOR_SPEED_PASSENGER_SEEK  {64};  // motor speed for turning around
const int MOTOR_SPEED_TURNING_AROUND  {-16};  // motor speed for following after initial passenger sighting

static int motorSpeedFollowing    { MOTOR_SPEED_FOLLOWING };      // current motor speed for following 
static int motorSpeedReversing    { MOTOR_SPEED_REVERSING };      // current motor speed for reversing
static int motorSpeedSeeking      { MOTOR_SPEED_SEEKING   };      // current motor speed for seeking
static int motorSpeedTurning      { MOTOR_SPEED_TURNING   };      // current motor speed for turning
static int *motorSpeeds[TapeFollow::numActions] {
        &motorSpeedFollowing,
        &motorSpeedReversing,
        &motorSpeedSeeking,
        &motorSpeedTurning
};
const int motorSpeedTurningAround { MOTOR_SPEED_TURNING_AROUND };
const int motorSpeedPassengerSeek { MOTOR_SPEED_PASSENGER_SEEK };

// General following variables
static bool active       {false};
static bool motorsActive {false};           // true if motors are active
static int control     {0};                  // current control parameter
static int motorSpeed  {motorSpeedSeeking};  // speed to add to motors
static float error     {0.};
static float lastError {0.};                 // last calculated error

// Action and counters
static TFAction action {TFAction::SEEKING};           // what the tapefollowing is currently doing (i.e. following, seeking, etc)
static long steps[TapeFollow::numActions];             // number of steps for associated action
static long onTapeCounter  [TapeFollow::numSensors];   // counts the number of consecutive onTape reads for each pin
static long offTapeCounter [TapeFollow::numSensors];   // counts the number of consecutive offTape reads for each pin
static long turnAroundPhaseCounter {0};                // counter for the turn around forward/reverse phase
static long turnAroundCounter      {0};                // counter for total steps turning around

// Readings
static int activePins[TapeFollow::numSensors];       // pin numbers (intL, mainL, mainR, intR)
static bitset<TapeFollow::numSensors> pinReadings;   // current readings on QRD pins
static vector<float>         errorArray {0., 0.};    // array of last 2 distinct errors
static vector<unsigned long> etimeArray {0,  1};     // array of times (since read) assoc with errorArray
static bitset<2> intersectSeen;                      // true if an intersection was seen
static bitset<2> intersectDetect;                    // true when an intersection has been detected (seen and passed over)

// Turning variables
static bool turningAround  {false};                 // true if the robot is turning around
static bool willTurnAround {false};                 // true if the robot is about to turn around
static bool halfTurn       {false};                 // if true, bot has turned far enough that mains are off tape
static Direction turnDirection {Direction::FRONT};  // current turn direction
static float leftWeight     {1.};                   // TODO: come up with a better way of doing this weight stuff
static float straightWeight {1.};
static float rightWeight    {1.};

// Functions
using setErrorFn_t = void (*) ();
static setErrorFn_t setErrorFunction[TapeFollow::numActions] {
        &TapeFollow::setErrorFollowing,
        &TapeFollow::setErrorReversing,
        &TapeFollow::setErrorSeeking,
        &TapeFollow::setErrorTurning
};

using updateStateFn_t = void (*) ();
static updateStateFn_t updateStateFunction[TapeFollow::numActions] {
        &TapeFollow::updateStateFollowing,
        &TapeFollow::updateStateReversing,
        &TapeFollow::updateStateSeeking,
        &TapeFollow::updateStateTurning
};


void TapeFollow::init()
{
    turningAround       = false;
    willTurnAround      = false;
    halfTurn            = false;
    motorsActive        = false;

    action              = TFAction::FOLLOWING;

    turnDirection       = Direction::FRONT;  // not turning
    control             = 0;

    for (int i(0); i < TapeFollow::numActions; ++i)
        *(motorSpeeds[i]) = DEFAULT_MOTOR_SPEEDS[i];
    motorSpeed          = 0;

    error               = 0.;
    lastError           = 0.;

    intersectSeen.reset();    // 00
    intersectDetect.reset();  // 00
    pinReadings.reset();      // 0000

    etimeArray          = {0,  1};
    errorArray          = {0., 0.};

    for (int i(0); i < TapeFollow::numSensors; ++i) {
        // reset counters
        onTapeCounter[i] = 0;
        offTapeCounter[i] = 0;
        // assign active pins
        activePins[i] = TapeFollow::tapeSensorsFront[i];
    }

    for (int i(0); i < TapeFollow::numActions; ++i)
        steps[i] = 0;

    leftWeight = 0.;
    rightWeight = 0.;
    straightWeight = 0.;

    // declare active pins as inputs
    for (int i(0); i < TapeFollow::numSensors; ++i)
        pinMode(activePins[i], INPUT);
}


bool TapeFollow::offTape()
{
    for (int i(0); i < TapeFollow::numSensors; ++i)
        if (offTapeCounter[i] < OFF_TAPE_PERIOD)
            return false;
    return true;
}


bool TapeFollow::mainsOffTape()
{
    for (int i(0); i < 2; ++i)
        if (offTapeCounter[i+1] < OFF_TAPE_PERIOD)
            return false;
    return true;
}


// TODO
void TapeFollow::updateIntersectionsSeen()
{
    bool intSeenL = (!mainsOffTape() &&
                     (onTapeCounter[0] >= INTERSECT_DETECT_PERIOD));
    bool intSeenR = (!mainsOffTape() &&
                     (onTapeCounter[3] >= INTERSECT_DETECT_PERIOD));

    // if seen, update instance variable
    if (intSeenL)
        intersectSeen[0] = 1;
    if (intSeenR)
        intersectSeen[1] = 1;
}


void TapeFollow::updateIntersectionsDetected()
{
    // check if intersections seen
    updateIntersectionsSeen();

    // check if intersection detected
    for (int i(0); i < 2; ++i)
        intersectDetect[i] = (!pinReadings[i]) && intersectSeen[i];
}


Direction TapeFollow::chooseTurnDeterministic(
        bool left, bool right, bool straight)
{ 
    int prefLeft = left * (
            (leftWeight > rightWeight) +
            (leftWeight > straightWeight));
    int prefRight = right * (
            (rightWeight > leftWeight) +
            (rightWeight > straightWeight));
    int prefStraight = straight * (
            (straightWeight > leftWeight) +
            (straightWeight > rightWeight));

    if (straight && (prefStraight >= prefLeft) && (prefStraight >= prefRight))
        return Direction::FRONT;
    else if (left && (prefLeft >= prefRight) && (prefLeft >= prefStraight))
        return Direction::LEFT;
    else if (right && (prefRight >= prefLeft) && (prefRight >= prefStraight))
        return Direction::RIGHT;
    else if (straight)
        return Direction::FRONT;
    else if (left)
        return Direction::LEFT;
    else if (right)
        return Direction::RIGHT;
    else
        return Direction::FRONT;
}


// TODO: Allow specifying probabilities from outside
Direction TapeFollow::chooseTurn(bool left, bool right, bool straight)
{
    float total = (
            left     * leftWeight +
            right    * rightWeight +
            straight * straightWeight
    );
    float leftProb;
    float rightProb;

    if (total == 0) {
        leftProb  = left  / (left + right + straight);
        rightProb = right / (left + right + straight);
    } else {
        leftProb  = left  * leftWeight  / total;
        rightProb = right * rightWeight / total;
    }

    // TODO: do this randValue part differently?
    float randValue = static_cast<float>(random(RANDOM_MAX_VAL)) /
        (RANDOM_MAX_VAL+1);
    float leftMax = 0 + leftProb;
    float rightMax = leftProb + rightProb;

    leftWeight = 0.;
    rightWeight = 0.;
    straightWeight = 0.;

    if (randValue < leftMax) 
        return Direction::LEFT;
    else if (randValue < rightMax) 
        return Direction::RIGHT;
    else 
        return Direction::FRONT;
}


void TapeFollow::setErrorFollowing()
{
    // Define aliases for the readings
    bool intrL = pinReadings[0];
    bool mainL = pinReadings[1];
    bool mainR = pinReadings[2];
    bool intrR = pinReadings[3];

    // determine error
    if (mainL && mainR)           // both main pins over tape
        error = 0.;
    else if (mainL)              // left main over tape
        error = ERROR_SMALL;
    else if (mainR)              // right main over tape
        error = -ERROR_SMALL;
    else if (intrL && (!intrR))   // left intersection over tape
        error = ERROR_MEDIUM;
    else if (intrR && (!intrL))   // right intersection over tape
        error = -ERROR_MEDIUM;
    else if (lastError < 0.)     // off tape to the right
        error = -ERROR_LARGE;
    else if (lastError > 0.)     // off tape to the left
        error = ERROR_LARGE;
    else 
        error = 0.;
}


// TODO: !!! For now this is the same as setErrorFollowing
void TapeFollow::setErrorReversing()
{
    TapeFollow::setErrorFollowing();
}


// TODO: make this more advanced
void TapeFollow::setErrorSeeking()
{
    if (lastError < 0.)              // off tape to the right
        error = -ERROR_SEEKING;
    else if (lastError > 0.)         // off tape to the left
        error = ERROR_SEEKING;
    else
        error = 0.;
}


// TODO: is this correct?
void TapeFollow::setErrorTurning()
{
    error = -(static_cast<int>(turnDirection)-1) * ERROR_TURNING;
}


void TapeFollow::setError()
{
    setErrorFunction[static_cast<int>(action)]();
    error *= abs(motorSpeedFollowing);  // TODO

    // update previous error parameters
    if (error != lastError) {
        errorArray = {lastError, errorArray[0]};
        etimeArray = {0,         etimeArray[0]};
        lastError  = error;
    }

    // increase time counters
    for (auto &t : etimeArray)
        ++t;
}


void TapeFollow::setControl()
{
    // get error derivatives
    float der1[2];
    der1[0] = (error - errorArray[0]) /
        static_cast<float>(etimeArray[0]);
    der1[1] = (errorArray[0] - errorArray[1]) /
        static_cast<float>(etimeArray[1] - etimeArray[0]);
    float der2 = (der1[0] - der1[1]) / static_cast<float>(etimeArray[0]);
    
    // get the effect of gains
    float ctrlProp (GAIN_PROP * error);
    float ctrlDer1 (GAIN_DER1 * der1[0]);
    float ctrlDer2 (GAIN_DER2 * der2);
    control = -static_cast<int>(ctrlProp + ctrlDer1 + ctrlDer2);

    int controlMax = abs(motorSpeedFollowing) * 3 / 2;  // TODO
    if (control > controlMax)
        control = controlMax;
    else if (control < -controlMax)
        control = -controlMax;
}


// TODO: !!!
void TapeFollow::updateStateFollowing()
{
    int followSteps  = steps[static_cast<int>(TFAction::FOLLOWING)];
    if (offTape())
        action = TFAction::SEEKING;
    else if (willTurnAround)
        action = TFAction::REVERSING;
    else if (followSteps >= INTERSECT_SEEK_DELAY_PERIOD) {
        updateIntersectionsDetected();
        // if intersection(s) detected, make turn decision
        if (intersectDetect.any() &&
            (offTapeCounter[0] >= PRE_TURN_DELAY_PERIOD) &&
            (offTapeCounter[3] >= PRE_TURN_DELAY_PERIOD)) {
            
            // TODO: do this a better way
            turnDirection = chooseTurnDeterministic(  // TODO: specify this function from major mode
                    intersectDetect[0], intersectDetect[1],
                    !mainsOffTape()
            );
            if (turnDirection != Direction::FRONT)  // TODO: move to updateSTate?
                action = TFAction::TURNING;
            
            // reset intersection arrays
            intersectSeen.reset();    // 00
            intersectDetect.reset();  // 00
        }
    }
}


// TODO: !!!
void TapeFollow::updateStateReversing()
{
    int reverseSteps = steps[static_cast<int>(TFAction::REVERSING)];
    if (offTape())
        action = TFAction::SEEKING;
    else if (willTurnAround) {
        if (reverseSteps >= PRE_TURN_AROUND_DELAY_PERIOD) {
            motorSpeedFollowing = MOTOR_SPEED_FOLLOWING;
            motorSpeedTurning = motorSpeedTurningAround;
            willTurnAround = false;
            turningAround = true;
            turnAroundCounter = 0;
            action = TFAction::TURNING;
        }
    }
}


// TODO: !!!
void TapeFollow::updateStateSeeking()
{
    if (!offTape()) {
        if (!willTurnAround)
            action = TFAction::FOLLOWING;
        else
            action = TFAction::REVERSING;
    }
}


// TODO: !!!
void TapeFollow::updateStateTurning()
{
    if ((!halfTurn) &&
        (offTapeCounter[1] >= TURN_CONFIRM_PERIOD) &&
        (offTapeCounter[2] >= TURN_CONFIRM_PERIOD)) {  // turn begins
        halfTurn = true;
    } else if (
            halfTurn &&
            ((onTapeCounter[1] >= ON_TAPE_PERIOD) ||
             (onTapeCounter[2] >= ON_TAPE_PERIOD))) {  // turn ends
        willTurnAround = false;
        halfTurn = false;
        action = TFAction::FOLLOWING; // exit to regular following
        turningAround = false;
        turnDirection = Direction::FRONT;
        motorSpeedTurning = MOTOR_SPEED_TURNING;
        motorSpeedFollowing = MOTOR_SPEED_FOLLOWING;
    } else if (turningAround) {  // during turn around
        if (turnAroundPhaseCounter >= TURN_AROUND_SPEED_SWITCH_PERIOD) {
            motorSpeedTurning = -motorSpeedTurning;
            turnAroundPhaseCounter = 0;
        } else if (turnAroundCounter >= TURN_AROUND_TIMEOUT_PERIOD) {
            turnAroundCounter = 0;
            if (turnDirection == Direction::LEFT)
                turnDirection = Direction::RIGHT;
            else
                turnDirection = Direction::LEFT;
        } else {
            ++turnAroundCounter;
            ++turnAroundPhaseCounter;
        }
    }
}


void TapeFollow::updateState()
{
    updateStateFunction[static_cast<int>(action)]();
}


void TapeFollow::updateCounters()
{
    // update steps counters
    for (int i(0); i < TapeFollow::numActions; ++i) {
        if (action != TapeFollow::allActions[i]) 
            steps[i] = 0;
        else if (steps[i] < COUNTER_MAX)
            steps[i] += 1;
    }
    
    // update on/off tape counters
    for (int i(0); i < TapeFollow::numSensors; ++i) {
        if (pinReadings[i]) {
            offTapeCounter[i] = 0;
            if (onTapeCounter[i] < COUNTER_MAX)
                onTapeCounter[i] += 1;
        } else {
            onTapeCounter[i] = 0;
            if (offTapeCounter[i] < COUNTER_MAX)
                offTapeCounter[i] += 1;
        }
    }
}


void TapeFollow::loop()
{
    // get readings from tape sensors
    for (int i(0); i < TapeFollow::numSensors; ++i) 
        pinReadings[i] = digitalRead(activePins[i]);

    // update counters
    updateCounters();

    // update state variables
    updateState();

    // set motor speed based on action
    motorSpeed = *(motorSpeeds[static_cast<int>(action)]);

    // get error based on current state
    setError();

    // get the control to apply to the motor speed
    setControl();

    // adjust motor speed
    if (motorsActive) {
        motor.speed(TapeFollow::motorPinL, control - motorSpeed);
        motor.speed(TapeFollow::motorPinR, control + motorSpeed);
    } else {
        motor.speed(TapeFollow::motorPinL, 0);
        motor.speed(TapeFollow::motorPinR, 0);
    }
}


void TapeFollow::start()
{
    active = true;
    motorsActive = true;
}


void TapeFollow::pause()
{
    active = false;
    motor.speed(TapeFollow::motorPinL, 0);
    motor.speed(TapeFollow::motorPinR, 0);
    motorsActive = false;
}


void TapeFollow::test()
{
    active = true;
    pause();
}


void TapeFollow::printLCD()
{
    LCD.clear();
    // print letter
    switch (action) {
        case TFAction::SEEKING:
            LCD.print( F("S ") );  // seeking
            break;
        case TFAction::TURNING:
            LCD.print( F("T ") );  // turning
            break;
        case TFAction::FOLLOWING:
            LCD.print( F("F ") );  // following
            break;
        case TFAction::REVERSING:
            LCD.print( F("R ") );
            break;
        default:
            LCD.print( F("? ") );
            break;
    }

    // print arrow
    if (turningAround)
        LCD.print( F("v") );
    else if (action == TFAction::TURNING) {
        switch (turnDirection) {
            case Direction::LEFT:
                LCD.print( F("<") );
                break;
            case Direction::FRONT:
                LCD.print( F("^") );
                break;
            case Direction::RIGHT:
                LCD.print( F(">") );
                break;
            case Direction::BACK:
                LCD.print( F("v") );
                break;
        }
    } else {
        if (control < 0)
            LCD.print( F("<") );
        else if (control > 0)
            LCD.print( F(">") );
        else
            LCD.print( F("^") );
    }

    // print QRD readings
    for (int i(0); i < TapeFollow::numSensors; ++i) {
        LCD.print( F(" ") );
        LCD.print(pinReadings[i]);
    }

    // print gains and control
    LCD.setCursor(0,1);
    LCD.print(GAIN_PROP);
    // LCD.print(errorArray[0]);
    LCD.print( F(" ") );
    LCD.print(GAIN_DER1);
    // LCD.print(errorArray[1]);
    LCD.print( F(" ") );
    LCD.print(control);
}


// TODO: !!! find a better way
void TapeFollow::setMotorSpeedPassengerSeek()
{
    motorSpeedFollowing = motorSpeedPassengerSeek;
}


// TODO: !!! find a better way
void TapeFollow::resetMotorSpeed()
{
    motorSpeedFollowing = MOTOR_SPEED_FOLLOWING;
}


// TODO: !!! check if this is correct
void TapeFollow::turnAround()
{
    if (!turningAround) {
        switch (turnDirection) {
            case Direction::LEFT:
                turnDirection = Direction::RIGHT;
                break;
            case Direction::RIGHT:
                turnDirection = Direction::LEFT;
                break;
            case Direction::FRONT:
                turnDirection = Direction::RIGHT;  // TODO: make a smarter way of choosing this
                break;
            case Direction::BACK:
                turnDirection = Direction::RIGHT;  // TODO: make a smarter way of choosing this
                break;
        }
        willTurnAround = true;
    }
}


bool TapeFollow::isTurning()
{
    return action == TFAction::TURNING;
}


bool TapeFollow::isSeeking()
{
    return action == TFAction::SEEKING;
}


void TapeFollow::giveTurnDirection(float left, float right, float straight)
{
    leftWeight = left;
    rightWeight = right;
    straightWeight = straight;
}
