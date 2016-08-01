#include <StandardCplusplus.h>
#include <phys253.h>
#include "TapeFollow.hpp"
#include "pins.hpp"
#include "PassengerSeek.hpp"

const int MOTOR_SPEED_FOLLOWING       {120};
const int MOTOR_SPEED_PASSENGER_SEEK  {64};
const int MOTOR_SPEED_TURNING         {32};
const int MOTOR_SPEED_TURNING_AROUND  {-8};
const int MOTOR_SPEED_SEEKING          {8};
const int MOTOR_SPEED_REVERSE        {-64};
const int PRINT_PERIOD {200};
const int PRE_TURN_AROUND_DELAY_PERIOD {145};
const unsigned long INTERSECT_DELAY {100};  // steps following before intersection seeking
const unsigned long MAIN_LOOP_DELAY {1};     // milliseconds
const double ERROR_SMALL   {.02};
const double ERROR_MEDIUM  {.04};
const double ERROR_LARGE   {.08};
const double ERROR_SEEKING {.64};
const double ERROR_TURNING {12.80};
const double EPSILON       {0.1};
const double GAIN_PROP     {7};
const double GAIN_DER1     {12};
const double GAIN_DER2     {.5*GAIN_DER1*GAIN_DER1/GAIN_PROP*(1.-EPSILON)};
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

bool pinReadings [4];     // current readings on QRD pins
bool lastPinReadings [NUM_SAVED_READINGS][4];

double gainProp = GAIN_PROP;        // TODO: set const; set based on knobs for now
double gainDer1 = GAIN_DER1;        // TODO: set const; set based on knobs for now
double gainDer2 = GAIN_DER2;  

bool active;                  // whether the loop is active
bool onTape;                  // true= on tape, false= off tape
bool lastOnTape;              // last value of onTape
bool mainsOnTape;             // whether one of the mains in on the tape
bool lastMainsOnTape;         // whether one of the mains was on the tape in the last step
bool turning;                 // true= turning, false= straight
bool halfTurn;                // if true, bot has turned far enough that mains are off tape
bool motorsActive;            // true if motors are active

Direction turnDirection;            // current direction (-1:left, 0:straight, 1:right)
int control;                  // current control parameter
int printCount;
int motorSpeed;               // speed to add to motors
int motorSpeedFollowing;
int motorSpeedTurning;
int tapeFollowSteps;
double lastError;             // last calculated error

bool intersectSeen [2] = {false, false};        // true if an intersection was seen
bool intersectDetect [2];// = {false, false};      // true when an intersection has been detected (seen and passed over)

double errorArray [2] = {0,  1};         // array of last 2 distinct errors
unsigned long etimeArray [2] = {0., 0.};   // array of times (since read) assoc with errorArray
int activePins [4];

bool willTurnAround;
bool turningAround;

float leftWeight;
float rightWeight;
float straightWeight;

void tapeFollowInit()
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
    motorSpeedTurning = MOTOR_SPEED_TURNING;
    motorSpeed      = motorSpeedFollowing;
    tapeFollowSteps = 0;

    lastError       = 0.;

    int i;
    int j;
    for(i = 0; i < NUM_SAVED_READINGS; i++){
      for(j = 0; j < 4; j++){
        lastPinReadings[i][j] = false;
      }
    }

    // assign active pins
    for (i = 0; i < 4; i++) 
      activePins[i] = TAPE_SENSORS_FRONT[i];

    // declare active pins as inputs
    for (i = 0; i < 4; i++)
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
double seekTape()
{
    if (lastError < 0.)              // off tape to the right
  return -ERROR_SEEKING;
    else if (lastError > 0.)         // off tape to the left
  return ERROR_SEEKING;
    else
  return 0.;
}


// TODO
void intersectionSeen()
{
    // bool intersectSeenL = true;
    bool intersectSeenL = true;
    bool intersectSeenR = true;
    for (int i = 0; i < NUM_SAVED_READINGS; i++) {
      if (i >= INTERSECT_PERIOD)
        break;
      intersectSeenL = (intersectSeenL && lastPinReadings[i][0] && mainsOnTape); //TODO:lastPinReadings must be producing true when it shouldn't  
      intersectSeenR = (intersectSeenR && lastPinReadings[i][3] && mainsOnTape);
    }

    // if seen, update instance variable
    if (intersectSeenL)
      intersectSeen[0] = true;
    if (intersectSeenR)
      intersectSeen[1] = true;
}


void intersectionDetection()
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


double followTape()
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


bool fnAllLastReadings(int period, int fn)
{    
    for (int i(0); i < period; ++i) {
      //auto *reading = lastPinReadings[i];
      bool reading [4];
      for(int j(0); j < 4; j++)
        reading[j] = lastPinReadings[i][j];
      bool result;
      if(fn == FN_MAINS_OFF_TAPE){
        result = mainsOffTape(reading);
      }else if(fn == FN_OFF_TAPE){
        result = offTape(reading);
      }else if(fn == FN_INTS_OFF_TAPE){
        result = intsOffTape(reading);
      }else{
        result = false; //YOU SHOULD NEVER GET HERE
      }
      if (!result)
        return false;
    }
    return true;
}


bool fnAnyLastReadings(int period, int fn)
{
    for (int i(0); i < period; ++i) {
    bool reading [4];
      for(int j(0); j < 4; j++)
        reading[j] = lastPinReadings[i][j];
    bool result;
    if(fn == FN_MAINS_OFF_TAPE){
        result = mainsOffTape(reading);
      }else if(fn == FN_OFF_TAPE){
        result = offTape(reading);
      }else if(fn == FN_INTS_OFF_TAPE){
        result = intsOffTape(reading);
      }else{
        result = false; //YOU SHOULD NEVER GET HERE
      }
    if (result)
      return true;
    }
    return false;
}


bool offTape(bool reading[])
{
    int i;
    for (i = 0; i < 4; i++)
      if (reading[i])
        return false;
    return true;
}


bool mainsOffTape(bool *reading)
{
    return !(reading[1] || reading[2]);
}


bool intsOffTape(bool *reading)
{
    return !(reading[0] || reading[3]);
}


bool intLOnTape(bool *reading)
{
    return reading[0];
}


bool intROnTape(bool *reading)
{
    return reading[3];
}


// TODO make more advanced
double makeTurn()
{
    // determine whether end has bee reached
    if ((!halfTurn) && fnAllLastReadings(
            TURNING_PERIOD, FN_MAINS_OFF_TAPE))
      halfTurn = true;
    else if (halfTurn && !(fnAnyLastReadings(
            ON_TAPE_PERIOD, FN_MAINS_OFF_TAPE))) {
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

Direction chooseTurn(bool left, bool right, bool straight)
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
  straightProb    = straight    / (left + right + straight) * 100.;
    } else {
  leftProb     = left     * leftWeight     / total * 100.;
  straightProb    = straight    * straightWeight    / total * 100.;
    }

    // TODO: do this randValue part differently?
    float randValue = (static_cast<float>(random(1000))) / 10.;
    float leftMax = 0 + leftProb;
    float straightMax = leftProb + straightProb;

    Serial.print("rand: ");
    Serial.println(randValue);
    Serial.print("left: ");
    Serial.println(leftMax);
    Serial.print("straight: ");
    Serial.println(straightMax);
    Serial.print("direction: ");
    
    leftWeight = 50.;
    rightWeight = 50.;
    straightWeight = 50.;

    if (randValue < leftMax){
      Serial.println("left");
      return Direction::LEFT;}
    else if (randValue < straightMax){
      Serial.println("straight"); 
      return Direction::FRONT;}
    else{
      Serial.println("right"); 
      return Direction::RIGHT;}
}

void printLCD()
{
    if (!active) {
      LCD.clear();
      LCD.print("why");
      LCD.setCursor(0,1);
      LCD.print("begin");
    } else {
      LCD.clear();
      // print letter
      if (!(turning || onTape))
          LCD.print("S ");  // seeking
      else if (turning)
          if(turningAround){
            LCD.print("TA");
          }else
            LCD.print("T ");  // turning
      else
          LCD.print("F ");  // following
      // print arrow
      if (turning) {
          if (turnDirection == Direction::LEFT)
        LCD.print("<--");
          else if (turnDirection == Direction::RIGHT)
        LCD.print("-->");
          else
        LCD.print(" ^ ");
      } else {
          if (control < 0)
        LCD.print("<  ");
          else if (control > 0)
        LCD.print("  >");
          else
        LCD.print(" ^ ");
      }
      // print QRD readings
      for (const auto read : pinReadings) {
          LCD.print(" ");
          LCD.print(read);
      }
      // print gains and control
      LCD.setCursor(0,1);
      LCD.print(gainProp);
      LCD.print(" ");
      LCD.print(gainDer1);
      LCD.print(" ");
      LCD.print(control);
    }
}

void tapeFollowLoop()
{
    if (!active){
      LCD.clear();
      LCD.print("in TF loop");
      return;
    }

    if (printCount % PRINT_PERIOD == 0) {
  printLCD();
  //PassengerSeek::printLCD();
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
    for (auto i(0); i < 4; ++i) {
  pinReadings[i] = static_cast<bool>(
                digitalRead(activePins[i]));
    }

    // update lastPinReadings array
    for(auto i(NUM_SAVED_READINGS-1); i>0; i--){
      for(auto j(0); j<4; j++){
        lastPinReadings[i][j] = lastPinReadings[i-1][j];
      }
    }
    for(auto i(0); i<4; i++){
      lastPinReadings[0][i] = pinReadings[i];
    }
    
    // determine whether on tape
    lastOnTape = onTape;

    bool isOnTape(false);
    for (const auto read : pinReadings) 
  if (read) {
      isOnTape = true;
      break;
  }
    onTape = isOnTape;

    // determine whether mains on tape
    lastMainsOnTape = mainsOnTape;
    mainsOnTape = (pinReadings[1] || pinReadings[2]);

    // get error based on current state
    double error(0.);
    if ((!(turning)) && fnAllLastReadings(
            OFF_TAPE_PERIOD, FN_OFF_TAPE)) {
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
    double der1[2];
    der1[0] = (error - errorArray[0]) /
        static_cast<double>(etimeArray[0]);
    der1[1] = (errorArray[0] - errorArray[1]) /
      static_cast<double>(etimeArray[1] - etimeArray[0]);
    double der2 = (der1[0] - der1[1]) /
      static_cast<double>(etimeArray[0]);

    // get the effect of gains
    double ctrlProp (gainProp * error);
    double ctrlDer1 (gainDer1 * der1[0]);
    double ctrlDer2 (gainDer2 * der2);
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
    for (auto &t : etimeArray)
  ++t;

    // delay
    delay(MAIN_LOOP_DELAY);
}

void turnAround()
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

void backUp(){
  
}

void tapeFollowStart()
{
    active = true;
    motorsActive = true;
}


void tapeFollowStop()
{
    tapeFollowInit();
    tapeFollowPause();
}


void tapeFollowPause()
{
    active = false;
    motorsActive = false;
    printLCD();
}


bool isActive()
{
    return active;
}


void tapeFollowTest()
{
    active = true;
    motorsActive = false;
}

void giveTurnDirection(float left, float right, float straight){
    leftWeight = left;
    rightWeight = right;
    straightWeight = straight;
}
