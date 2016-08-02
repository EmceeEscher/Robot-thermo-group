///
// TapeFollow.cpp
//
#include <StandardCplusplus.h>
#include <phys253.h>
#include "TapeFollow.hpp"
#include "pins.hpp"
#include "PassengerSeek.hpp"
#include "ToDestination.hpp"


const int MOTOR_SPEED_FOLLOWING       {120};
const int MOTOR_SPEED_PASSENGER_SEEK  {64};
const int MOTOR_SPEED_TURNING         {32};
const int MOTOR_SPEED_TURNING_AROUND  {-8};
const int MOTOR_SPEED_SEEKING          {8};
const int MOTOR_SPEED_REVERSE        {-64};
const int PRINT_PERIOD {200};
const int PRE_TURN_AROUND_DELAY_PERIOD {145};
const int INTERSECT_DELAY {100};  // steps following before intersection seeking
const unsigned long MAIN_LOOP_DELAY {1};     // milliseconds
const float ERROR_SMALL   {.02};
const float ERROR_MEDIUM  {.04};
const float ERROR_LARGE   {.08};
const float ERROR_SEEKING {.64};
const float ERROR_TURNING {12.80};
const float EPSILON       {0.1};
const float GAIN_PROP     {9};
const float GAIN_DER1     {12};
const float GAIN_DER2     {.5*GAIN_DER1*GAIN_DER1/GAIN_PROP*(1.-EPSILON)};
// const double GAIN_DER2 {0.};
const int NUM_SAVED_READINGS {52};
const int INTERSECT_PERIOD {5};  
const int TURNING_PERIOD   {10}; 
const int TURN_WAIT_PERIOD {45};
const int OFF_TAPE_PERIOD  {50};
const int ON_TAPE_PERIOD   {10};

const unsigned long RANDOM_MAX_VAL     {100000};

const int FN_MAINS_OFF_TAPE = 0;
const int FN_OFF_TAPE = 1;
const int FN_INTS_OFF_TAPE = 2;

static bool pinReadings [4];     // current readings on QRD pins
static bool lastPinReadings [NUM_SAVED_READINGS][4];

const float gainProp = GAIN_PROP;        // TODO: set const; set based on knobs for now
const float gainDer1 = GAIN_DER1;        // TODO: set const; set based on knobs for now
const float gainDer2 = GAIN_DER2;  

static bool active;                  // whether the loop is active
static bool onTape;                  // true= on tape, false= off tape
static bool lastOnTape;              // last value of onTape
static bool mainsOnTape;             // whether one of the mains in on the tape
static bool lastMainsOnTape;         // whether one of the mains was on the tape in the last step
static bool turning;                 // true= turning, false= straight
static bool halfTurn;                // if true, bot has turned far enough that mains are off tape
static bool motorsActive;            // true if motors are active

static Direction turnDirection;            // current direction (-1:left, 0:straight, 1:right)
static int control;                  // current control parameter
static int printCount;
static int motorSpeed;               // speed to add to motors
static int motorSpeedFollowing;
static int motorSpeedTurning;
static int tapeFollowSteps;
static float lastError;             // last calculated error

static bool intersectSeen [2] = {false, false};        // true if an intersection was seen
static bool intersectDetect [2];// = {false, false};      // true when an intersection has been detected (seen and passed over)

static float errorArray [2] = {0.,  0.};         // array of last 2 distinct errors
static unsigned long etimeArray [2] = {0, 1};   // array of times (since read) assoc with errorArray
static int activePins [4];

static bool willTurnAround;
static bool turningAround;

static float leftWeight;
static float rightWeight;
static float straightWeight;


namespace TapeFollow
{
    static float seekTape();
    static void intersectionSeen();
    static void intersectionDetection();
    static float followTape();
    static bool fnAllLastReadings(int period, int fn);
    static bool fnAnyLastReadings(int period, int fn);
    static bool offTape(bool reading[]);
    static bool mainsOffTape(bool reading[]);
    static bool intsOffTape(bool reading[]);
    // static bool intLOnTape(bool reading[]);
    // static bool intROnTape(bool reading[]);
    static float makeTurn();
    static Direction chooseTurn(bool left, bool right, bool straight);
}


void TapeFollow::init()
{
    active          = false;
    onTape          = false;
    lastOnTape      = false;
    mainsOnTape     = false;
    lastMainsOnTape = false;
    turning         = false;
    halfTurn        = false;
    motorsActive    = false;
    
    turnDirection   = Direction::FRONT;
    control         = 0;
    printCount      = 0;
    motorSpeedFollowing = MOTOR_SPEED_FOLLOWING;
    motorSpeedTurning   = MOTOR_SPEED_TURNING;
    motorSpeed          = motorSpeedFollowing;
    tapeFollowSteps = 0;
    
    lastError       = 0.;
    
    for (int i = 0; i < NUM_SAVED_READINGS; ++i) 
        for (int j = 0; j < 4; ++j) 
            lastPinReadings[i][j] = false;
    
    // assign active pins
    for (int i = 0; i < 4; ++i) 
        activePins[i] = TAPE_SENSORS_FRONT[i];
    
    // declare active pins as inputs
    for (int i = 0; i < 4; ++i)
        pinMode(activePins[i], INPUT);
    
    willTurnAround = false;
    turningAround = false;
    
    intersectDetect[0] = false;
    intersectDetect[1] = false;
    
    leftWeight = 50.;
    rightWeight = 50.;
    straightWeight = 50.;
}


// TODO make this more advanced
float TapeFollow::seekTape()
{
    if (lastError < 0.)              // off tape to the right
        return -ERROR_SEEKING;
    else if (lastError > 0.)         // off tape to the left
        return ERROR_SEEKING;
    else
        return 0.;
}


// TODO
void TapeFollow::intersectionSeen()
{
    // bool intersectSeenL = true;
    bool intersectSeenL = true;
    bool intersectSeenR = true;
    for (int i = 0; i < NUM_SAVED_READINGS; ++i) {
        if (i >= INTERSECT_PERIOD)
            break;
        intersectSeenL = (
                intersectSeenL && lastPinReadings[i][0] && mainsOnTape); //TODO:lastPinReadings must be producing true when it shouldn't  
        intersectSeenR = (
                intersectSeenR && lastPinReadings[i][3] && mainsOnTape);
    }
    
    // if seen, update instance variable
    if (intersectSeenL)
        intersectSeen[0] = true;
    if (intersectSeenR)
        intersectSeen[1] = true;
}


void TapeFollow::intersectionDetection()
{
    // declare static variables (runs once)
    bool intersectL = pinReadings[0];
    bool mainL      = pinReadings[1];
    bool mainR      = pinReadings[2];
    bool intersectR = pinReadings[3];
    
    // check if intersections seen
    intersectionSeen();
    
    // check if intersection detected
    if ((!intersectL) && intersectSeen[0])
        intersectDetect[0] = true;
    if ((!intersectR) && intersectSeen[1])
        intersectDetect[1] = true;
    
    // if intersection(s) detected, make move decision
    if (fnAllLastReadings(TURN_WAIT_PERIOD, FN_INTS_OFF_TAPE) &&
        (intersectDetect[0] || intersectDetect[1])) {
        // wait until both intersections crossed over
        turnDirection = chooseTurn(
                intersectDetect[0],
                intersectDetect[1],
                (mainL || mainR)
        ); 
        if (turnDirection != Direction::FRONT)
            turning = true;  // activates `makeTurn` function
        else
            turning = false;
        // reset intersection arrays
        intersectSeen[0] = false;
        intersectSeen[1] = false;
        intersectDetect[0] = false;
        intersectDetect[1] = false;
    }
}


float TapeFollow::followTape()
{
    // declare static variables (runs once)
    bool intersectL = pinReadings[0];
    bool mainL      = pinReadings[1];
    bool mainR      = pinReadings[2];
    bool intersectR = pinReadings[3];
    
    if (tapeFollowSteps >= PRE_TURN_AROUND_DELAY_PERIOD && willTurnAround) {
        motorSpeedFollowing = MOTOR_SPEED_FOLLOWING;
        willTurnAround = false;
        turningAround = true;
        turning = true;
        turnDirection = Direction::RIGHT;
    }
    
    if (tapeFollowSteps >= INTERSECT_DELAY)
        intersectionDetection();
    
    // determine error
    if (mainL && mainR)                    // both pins over tape
        return 0.;
    else if (mainL)                       // left main over tape
        return ERROR_SMALL;
    else if (mainR)                       // right main over tape
        return -ERROR_SMALL;
    else if (intersectL && (!intersectR))  // left intersection over tape
        return ERROR_MEDIUM;
    else if (intersectR && (!intersectL))  // right intersection over tape
        return -ERROR_MEDIUM;
    else if (lastError < 0.)         // off tape to the right
        return -ERROR_LARGE;
    else if (lastError > 0.)         // off tape to the left
        return ERROR_LARGE;
    else
        return 0.;
}


bool TapeFollow::fnAllLastReadings(int period, int fn)
{    
    for (int i(0); i < period; ++i) {
        //auto *reading = lastPinReadings[i];
        bool reading [4];
        for (int j(0); j < 4; ++j)
            reading[j] = lastPinReadings[i][j];
        bool result;
        if (fn == FN_MAINS_OFF_TAPE)
            result = mainsOffTape(reading);
        else if (fn == FN_OFF_TAPE)
            result = offTape(reading);
        else if (fn == FN_INTS_OFF_TAPE)
            result = intsOffTape(reading);
        else
            result = false; //YOU SHOULD NEVER GET HERE
        
        if (!result)
            return false;
    }
    return true;
}


bool TapeFollow::fnAnyLastReadings(int period, int fn)
{
    for (int i(0); i < period; ++i) {
        bool reading [4];
        for (int j(0); j < 4; ++j)
            reading[j] = lastPinReadings[i][j];
        bool result;
        if (fn == FN_MAINS_OFF_TAPE) 
            result = mainsOffTape(reading);
        else if (fn == FN_OFF_TAPE)
            result = offTape(reading);
        else if (fn == FN_INTS_OFF_TAPE)
            result = intsOffTape(reading);
        else
            result = false; //YOU SHOULD NEVER GET HERE
        if (result)
            return true;
    }
    return false;
}


bool TapeFollow::offTape(bool reading[])
{
    for (int i = 0; i < 4; ++i)
        if (reading[i])
            return false;
    return true;
}


bool TapeFollow::mainsOffTape(bool *reading)
{
    return !(reading[1] || reading[2]);
}


bool TapeFollow::intsOffTape(bool *reading)
{
    return !(reading[0] || reading[3]);
}


// bool TapeFollow::intLOnTape(bool *reading)
// {
//     return reading[0];
// }


// bool TapeFollow::intROnTape(bool *reading)
// {
//     return reading[3];
// }


// TODO make more advanced
float TapeFollow::makeTurn()
{
    // determine whether end has bee reached
    if ((!halfTurn) && fnAllLastReadings(TURNING_PERIOD, FN_MAINS_OFF_TAPE))
        halfTurn = true;
    else if (halfTurn &&
             !(fnAnyLastReadings(ON_TAPE_PERIOD, FN_MAINS_OFF_TAPE))) {
        halfTurn = false;
        turning = false;
        turningAround = false; // exit to regular following
    }
    
    // determine error
    if (!turning)
        return 0.;
    else
        return -(static_cast<int>(turnDirection)-1) * ERROR_TURNING;
}


Direction TapeFollow::chooseTurn(bool left, bool right, bool straight)
{
    /*if(right)
      return Direction::RIGHT;
      else if(straight)
      return Direction::FRONT;
      else
      return Direction::LEFT;*/
    float total = (
            left     * leftWeight +
            right    * rightWeight +
            straight * straightWeight
    );
    
    float leftProb;
    float straightProb;
    if (total == 0) {
        leftProb     = left     / (left + right + straight) * 100.;
        straightProb = straight / (left + right + straight) * 100.;
    } else {
        leftProb     = left     * leftWeight     / total * 100.;
        straightProb = straight * straightWeight / total * 100.;
    }
    
    // TODO: do this randValue part differently?
    float randValue = (static_cast<float>(random(1000))) / 10.;
    float leftMax = 0 + leftProb;
    float straightMax = leftProb + straightProb;
    
    Serial.print( F("rand: ") );
    Serial.println(randValue);
    Serial.print( F("left: ") );
    Serial.println(leftMax);
    Serial.print( F("straight: ") );
    Serial.println(straightMax);
    Serial.print( F("direction: ") );
    
    leftWeight = 50.;
    rightWeight = 50.;
    straightWeight = 50.;
    
    if (randValue < leftMax){
        Serial.println( F("left") );
        return Direction::LEFT;}
    else if (randValue < straightMax){
        Serial.println( F("straight") ); 
        return Direction::FRONT;}
    else{
        Serial.println( F("right") ); 
        return Direction::RIGHT;}
}


void TapeFollow::printLCD()
{
    if (!active) {
        LCD.clear();
        LCD.print( F("why") );
        LCD.setCursor(0,1);
        LCD.print( F("begin") );
    } else {
        LCD.clear();
        // print letter
        if (!(turning || onTape))
            LCD.print( F("S ") );  // seeking
        else if (turning)
            if (turningAround)
                LCD.print( F("TA") );
            else
                LCD.print( F("T ") );  // turning
        else
            LCD.print( F("F ") );  // following
        // print arrow
        if (turning) {
            if (turnDirection == Direction::LEFT)
                LCD.print( F("<--") );
            else if (turnDirection == Direction::RIGHT)
                LCD.print( F("-->") );
            else
                LCD.print( F(" ^ ") );
        } else {
            if (control < 0)
                LCD.print( F("<  ") );
            else if (control > 0)
                LCD.print( F("  >") );
            else
                LCD.print( F(" ^ ") );
        }
        // print QRD readings
        for (int i(0); i < 4; ++i) {
            LCD.print( F(" ") );
            LCD.print(pinReadings[i]);
        }
        // print gains and control
        LCD.setCursor(0, 1);
        LCD.print(gainProp);
        LCD.print( F(" ") );
        LCD.print(gainDer1);
        LCD.print( F(" ") );
        LCD.print(control);
    }
}


void TapeFollow::loop()
{
    if (!active){
        LCD.clear();
        LCD.print( F("in TF loop") );
        return;
    }
    
    if (printCount % PRINT_PERIOD == 0) {
        //printLCD();
        PassengerSeek::printLCD();
        //detectBeaconPrintLCD();
        printCount = 0;
    }
    ++printCount;
    
    // set gains
    // TODO move this to constructor once values are decided upon
    /*   if (!motorsActive) {
         gainProp = static_cast<double>(knob(KNOB_PROP_GAIN)) / 50.;
         gainDer1 = static_cast<double>(knob(KNOB_DER1_GAIN)) / 50.;
         gainDer2 = .5*gainDer1*gainDer1 /
         gainProp*(1.-EPSILON);
         }*/
    
    // get readings from tape sensors
    for (int i(0); i < 4; ++i) 
        pinReadings[i] = static_cast<bool>(digitalRead(activePins[i]));
    
    // update lastPinReadings array
    for (int i(NUM_SAVED_READINGS-1); i > 0; --i) 
        for (int j(0); j < 4; ++j) 
            lastPinReadings[i][j] = lastPinReadings[i-1][j];
    
    for (int i(0); i < 4; ++i)
        lastPinReadings[0][i] = pinReadings[i];
    
    // determine whether on tape
    lastOnTape = onTape;
    
    bool isOnTape(false);
    for (int i(0); i < 4; ++i) {
        bool read = pinReadings[i];
        if (read) {
            isOnTape = true;
            break;
        }
    }
    onTape = isOnTape;
    
    // determine whether mains on tape
    lastMainsOnTape = mainsOnTape;
    mainsOnTape = (pinReadings[1] || pinReadings[2]);
    
    // get error based on current state
    float error(0.);
    if ((!(turning)) && fnAllLastReadings(OFF_TAPE_PERIOD, FN_OFF_TAPE)) {
        motorSpeed = motorSpeedTurning;
        tapeFollowSteps = 0;
        error = seekTape();
    } else if (turning) {
        motorSpeed = motorSpeedTurning;
        tapeFollowSteps = 0;
        error = makeTurn();
    } else {
        motorSpeed = motorSpeedFollowing;
        tapeFollowSteps += 1;
        error = followTape();
    }
    error *= MOTOR_SPEED_FOLLOWING; //TODO: check if this should be variable
    
    // update previous error parameters
    if (error != lastError) {
        errorArray[1] = errorArray[0];
        errorArray[0] = lastError;
        etimeArray[1] = etimeArray[0];
        etimeArray[0] = 1;
        lastError  = error;
    }
    
    // get error derivatives
    float der1[2];
    der1[0] = (error - errorArray[0]) /
        static_cast<float>(etimeArray[0]);
    der1[1] = (errorArray[0] - errorArray[1]) /
        static_cast<float>(etimeArray[1] - etimeArray[0]);
    float der2 = (der1[0] - der1[1]) /
        static_cast<float>(etimeArray[0]);
    
    // get the effect of gains
    float ctrlProp (gainProp * error);
    float ctrlDer1 (gainDer1 * der1[0]);
    float ctrlDer2 (gainDer2 * der2);
    control = -static_cast<int>(ctrlProp + ctrlDer1 + ctrlDer2);
    
    int controlMax = 1.5 * MOTOR_SPEED_FOLLOWING;
    if (control > controlMax)
        control = controlMax;
    else if (control < -controlMax)
        control = -controlMax;
    
    int dSpeed = control;
    
    // adjust motor speed
    if (motorsActive) {
        motor.speed(MOTOR_PIN_L, dSpeed - motorSpeed);
        motor.speed(MOTOR_PIN_R, dSpeed + motorSpeed);
    } else {
        motor.speed(MOTOR_PIN_L, 0);
        motor.speed(MOTOR_PIN_R, 0);
    }
    
    // increase time counters
    for (int i(0); i < 2; ++i)
        etimeArray[i] += 1;
    
    // delay
    delay(MAIN_LOOP_DELAY);
}


void TapeFollow::turnAround()
{
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
    motorSpeedTurning = MOTOR_SPEED_TURNING_AROUND;
    motorSpeedFollowing = MOTOR_SPEED_REVERSE;
    tapeFollowSteps = 0;  // reset steps counter
}


void TapeFollow::backUp() {}


void TapeFollow::start()
{
    active = true;
    motorsActive = true;
}


void TapeFollow::stop()
{
    TapeFollow::init();
    TapeFollow::pause();
}


void TapeFollow::pause()
{
    active = false;
    motorsActive = false;
    TapeFollow::printLCD();
}


bool TapeFollow::isActive()
{
    return active;
}


void TapeFollow::test()
{
    active = true;
    motorsActive = false;
}


void TapeFollow::giveTurnDirection(float left, float right, float straight)
{
    leftWeight = left;
    rightWeight = right;
    straightWeight = straight;
}
