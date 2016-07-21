///
// TapeFollow.cpp
//
#include <StandardCplusplus.h>
#include <vector>
#include <algorithm>
#include <phys253.h>
#include "myalgs.hpp"
#include "pins.hpp"
#include "Direction.hpp"
#include "TapeFollow.hpp"


using std::vector;


const int MOTOR_SPEED_FOLLOWING      {120};
const int MOTOR_SPEED_PASSENGER_SEEK  {64};
const int MOTOR_SPEED_TURNING         {32};
const int MOTOR_SPEED_SEEKING          {8};
const int PRINT_PERIOD {200};
const unsigned long INTERSECT_DELAY {100};  // steps following before intersection seeking
const double ERROR_SMALL     {.02};
const double ERROR_MEDIUM    {.04};
const double ERROR_LARGE     {.08};
const double ERROR_SEEKING   {.64};
const double ERROR_TURNING {12.80};
const double EPSILON         {.10};
const double GAIN_PROP      {4.68};
const double GAIN_DER1      {9.54};
const double GAIN_DER2     {.5*GAIN_DER1*GAIN_DER1/GAIN_PROP*(1.-EPSILON)};
// const double GAIN_DER2 {0.};
const int NUM_SAVED_READINGS {52};
const int INTERSECT_PERIOD    {5};  
const int TURNING_PERIOD     {10}; 
const int TURN_WAIT_PERIOD   {45};
const int OFF_TAPE_PERIOD    {50};
const int ON_TAPE_PERIOD     {10};


void TapeFollow::init()
{
    MinorMode::init();

    this->onTape              = false;
    this->lastOnTape          = false;
    this->mainsOnTape         = false;
    this->lastMainsOnTape     = false;
    this->seeking             = false;
    this->turning             = false;
    this->halfTurn            = false;
    this->motorsActive        = false;

    this->turnDirection       = Direction::FRONT;  // not turning
    this->control             = 0;
    this->printCount          = 0;
    this->motorSpeedFollowing = this->motorSpeedFollowingDefault;
    this->motorSpeed          = this->motorSpeedFollowing;
    this->tapeFollowSteps     = 0;

    this->lastError           = 0.;

    this->intersectSeen       = {false, false};
    this->intersectDetect     = {false, false};

    this->etimeArray          = {0,  1};
    this->errorArray          = {0., 0.};

    this->pinReadings         = {false, false, false, false};

    for (auto &x : this->lastPinReadings)
	std::fill(x.begin(), x.end(), false);

    // assign active pins
    std::copy(
            this->tapeSensorsFront.begin(),
	    this->tapeSensorsFront.end(),
	    this->activePins.begin()
    );
    // for (auto i(0); i < 4; ++i) 
    // 	this->activePins[i] = pins::TAPE_SENSORS_FRONT[i];

    // declare active pins as inputs
    for (const auto pin : this->activePins)
	pinMode(pin, INPUT);
}


// TODO make this more advanced
double TapeFollow::seekTape()
{
    if (this->lastError < 0.)              // off tape to the right
	return -this->errorSeeking;
    else if (this->lastError > 0.)         // off tape to the left
	return this->errorSeeking;
    else
	return 0.;
}


// TODO
void TapeFollow::intersectionSeen()
{
    // bool intersectSeenL = true;
    bool intersectSeenL(true);
    bool intersectSeenR(true);
    int i(0);
    for (const auto &reads : this->lastPinReadings) {
	if (i >= this->intersectPeriod)
	    break;
	intersectSeenL = (intersectSeenL && reads[0] && this->mainsOnTape);
	intersectSeenR = (intersectSeenR && reads[3] && this->mainsOnTape);
	++i;
    }

    // if seen, update instance variable
    if (intersectSeenL)
	this->intersectSeen[0] = true;
    if (intersectSeenR)
	this->intersectSeen[1] = true;
}


void TapeFollow::intersectionDetection()
{
    // declare static variables (runs once)
    const static bool &intersectL = this->pinReadings[0];
    const static bool &mainL      = this->pinReadings[1];
    const static bool &mainR      = this->pinReadings[2];
    const static bool &intersectR = this->pinReadings[3];

    // check if intersections seen
    this->intersectionSeen();

    // check if intersection detected
    if ((!intersectL) && this->intersectSeen[0])
	this->intersectDetect[0] = true;
    if ((!intersectR) && this->intersectSeen[1])
	this->intersectDetect[1] = true;

    // if intersection(s) detected, make move decision
    if (this->fnAllLastReadings(
            this->turnWaitPeriod, &TapeFollow::intsOffTape)) {
	// wait until both intersections crossed over
	this->turnDirection = this->chooseTurn(
	        this->intersectDetect[0],
		this->intersectDetect[1],
		(mainL || mainR)
        );
	if (this->turnDirection != Direction::FRONT)
	    this->turning = true;  // activates `makeTurn` function
	// reset intersection arrays
	this->intersectSeen =   {false, false};
	this->intersectDetect = {false, false};
    }
}


double TapeFollow::followTape()
{
    // declare static variables (runs once)
    const static bool &intersectL = this->pinReadings[0];
    const static bool &mainL      = this->pinReadings[1];
    const static bool &mainR      = this->pinReadings[2];
    const static bool &intersectR = this->pinReadings[3];

    if (this->tapeFollowSteps > this->intersectDelay)
	this->intersectionDetection();

    // determine error
    if (mainL && mainR)                    // both pins over tape
	return 0.;
    else if (mainL)                       // left main over tape
	return this->errorSmall;
    else if (mainR)                       // right main over tape
	return -this->errorSmall;
    else if (intersectL && (!intersectR))  // left intersection over tape
	return this->errorMedium;
    else if (intersectR && (!intersectL))  // right intersection over tape
	return -this->errorMedium;
    else if (this->lastError < 0.)         // off tape to the right
	return -this->errorLarge;
    else if (this->lastError > 0.)         // off tape to the left
	return this->errorLarge;
    else
	return 0.;
}


// TODO: just use `all_of` in place of this method
bool TapeFollow::fnAllLastReadings(int period, readingFn_t fn)
{
    return std::all_of(
            this->lastPinReadings.begin(),
	    this->lastPinReadings.begin()+period,
	    fn
    );
}


// TODO: just use `any_of` in place of this method
bool TapeFollow::fnAnyLastReadings(int period, readingFn_t fn)
{
    return std::any_of(
            this->lastPinReadings.begin(),
	    this->lastPinReadings.begin()+period,
	    fn
    );
}


bool TapeFollow::offTape(vector<bool> reading)
{
    for (const bool read : reading)
	if (read)
	    return false;
    return true;
}


bool TapeFollow::mainsOffTape(vector<bool> reading)
{
    return !(reading[1] || reading[2]);
}


bool TapeFollow::intsOffTape(vector<bool> reading)
{
    return !(reading[0] || reading[3]);
}


bool TapeFollow::intLOnTape(vector<bool> reading)
{
    return reading[0];
}


bool TapeFollow::intROnTape(vector<bool> reading)
{
    return reading[3];
}


// TODO make more advanced
double TapeFollow::makeTurn()
{
    // determine whether end has bee reached
    if ((!this->halfTurn) && this->fnAllLastReadings(
            this->turningPeriod, &TapeFollow::mainsOffTape))
    	this->halfTurn = true;
    else if (this->halfTurn && !(this->fnAnyLastReadings(
            this->onTapePeriod, &TapeFollow::mainsOffTape))) {
	this->halfTurn = false;
	this->turning = false;  // exit to regular following
    }

    // determine error
    if (!this->turning)
	return 0.;
    else
	return -(static_cast<int>(this->turnDirection)-1) * this->errorTurning;
}


// TODO: generalize
Direction TapeFollow::chooseTurn(bool left, bool right, bool straight)
{
    // // for now, random
    // if (left && right && straight)
    // 	return random(3) - 1;
    // else if (left && right)
    // 	return 2*random(2) - 1;
    // else if (left && straight)
    // 	return random(2) - 1;
    // else if (right && straight)
    // 	return random(2);
    // else if (left)
    // 	return -1;
    // else if (right)
    // 	return 1;
    // else
    // 	return 0;

    // for now, prefer left, then right, then straight
    if (left)
    	return Direction::LEFT;  // left
    else if (right)
    	return Direction::RIGHT;   // right
    else
    	return Direction::FRONT;   // straight
}


void TapeFollow::printLCD()
{
    if (!this->active) {
    	LCD.clear();
    	LCD.print("Press START to");
    	LCD.setCursor(0,1);
    	LCD.print("begin");
    } else {
    	LCD.clear();
    	// print letter
    	if (!(this->turning || this->onTape))
    	    LCD.print("S ");  // seeking
    	else if (this->turning)
    	    LCD.print("T ");  // turning
    	else
    	    LCD.print("F ");  // following
    	// print arrow
    	if (this->turning) {
	    switch (this->turnDirection) {
	    case Direction::LEFT:
		LCD.print("<--");
		break;
	    case Direction::FRONT:
		LCD.print(" ^ ");
		break;
	    case Direction::RIGHT:
		LCD.print("-->");
		break;
	    case Direction::BACK:
		LCD.print(" v ");
		break;
	    }
    	} else {
    	    if (this->control < 0)
    		LCD.print("<  ");
    	    else if (this->control > 0)
    		LCD.print("  >");
    	    else
    		LCD.print(" ^ ");
    	}
    	// print QRD readings
    	for (const auto read : this->pinReadings) {
    	    LCD.print(" ");
    	    LCD.print(read);
    	}
    	// print gains and control
    	LCD.setCursor(0,1);
    	LCD.print(this->gainProp);
    	LCD.print(" ");
    	LCD.print(this->gainDer1);
    	LCD.print(" ");
    	LCD.print(this->control);
    }
}


TapeFollow::TapeFollow()
    : MinorMode(),
      tapeSensorsFront (pins::TAPE_SENSORS_FRONT),
      tapeSensorsBack  (pins::TAPE_SENSORS_BACK),
      gainProp         (GAIN_PROP),
      gainDer1         (GAIN_DER1),
      gainDer2         (GAIN_DER2),
      errorSmall       (ERROR_SMALL),
      errorMedium      (ERROR_MEDIUM),
      errorLarge       (ERROR_LARGE),
      errorSeeking     (ERROR_SEEKING),
      errorTurning     (ERROR_TURNING),
      intersectDelay   (INTERSECT_DELAY),
      intersectPeriod  (INTERSECT_PERIOD),
      turningPeriod    (TURNING_PERIOD),
      turnWaitPeriod   (TURN_WAIT_PERIOD),
      offTapePeriod    (OFF_TAPE_PERIOD),
      onTapePeriod     (ON_TAPE_PERIOD),
      printPeriod      (PRINT_PERIOD),
      motorSpeedTurning          (MOTOR_SPEED_TURNING),
      motorSpeedSeeking          (MOTOR_SPEED_SEEKING),
      motorSpeedFollowingDefault (MOTOR_SPEED_FOLLOWING),
      motorSpeedPassengerSeek    (MOTOR_SPEED_PASSENGER_SEEK),
      pinReadings     (4, false),
      lastPinReadings (NUM_SAVED_READINGS, vector<bool>(4, false)),
      activePins      (4)
{
}


TapeFollow::~TapeFollow() {}


void TapeFollow::loop()
{
    if (!this->active)
	return;

    if (this->printCount % this->printPeriod == 0) {
	this->printLCD();
	this->printCount = 0;
    }
    ++this->printCount;

    // set gains
    // TODO move this to constructor once values are decided upon
    if (!this->motorsActive) {
	this->gainProp = static_cast<double>(knob(pins::KNOB_PROP_GAIN)) / 50.;
	this->gainDer1 = static_cast<double>(knob(pins::KNOB_DER1_GAIN)) / 50.;
	this->gainDer2 = .5*this->gainDer1*this->gainDer1 /
	        this->gainProp*(1.-EPSILON);
    }

    // get readings from tape sensors
    for (auto i(0); i < 4; ++i) {
	this->pinReadings[i] = static_cast<bool>(
                digitalRead(this->activePins[i]));
    }

    // update lastPinReadings array
    std::rotate(
            this->lastPinReadings.begin(), this->lastPinReadings.end()-1,
	    this->lastPinReadings.end());
    std::copy(
            this->pinReadings.begin(), this->pinReadings.end(),
	    this->lastPinReadings.front().begin());
    
    // determine whether on tape
    this->lastOnTape = this->onTape;

    bool isOnTape(false);
    for (const auto read : this->pinReadings) 
	if (read) {
	    isOnTape = true;
	    break;
	}
    this->onTape = isOnTape;

    // determine whether mains on tape
    this->lastMainsOnTape = this->mainsOnTape;
    this->mainsOnTape = (this->pinReadings[1] || this->pinReadings[2]);

    // get error based on current state
    this->seeking = (!this->turning) &&
	(this->fnAllLastReadings(this->offTapePeriod, &TapeFollow::offTape));
    double error(0.);
    if (this->seeking) {
	this->motorSpeed = this->motorSpeedSeeking;
	this->tapeFollowSteps = 0;
	error = this->seekTape();
    } else if (this->turning) {
	this->motorSpeed = this->motorSpeedTurning;
	this->tapeFollowSteps = 0;
	error = makeTurn();
    } else {
	this->motorSpeed = this->motorSpeedFollowing;
	this->tapeFollowSteps += 1;
	error = followTape();
    }
    error *= this->motorSpeedFollowing;

    // update previous error parameters
    if (error != this->lastError) {
	this->errorArray = {this->lastError, this->errorArray[0]};
	this->etimeArray = {1,              this->etimeArray[0]};
	this->lastError  = error;
    }

    // get error derivatives
    double der1[2];
    der1[0] = (error - this->errorArray[0]) /
  	    static_cast<double>(this->etimeArray[0]);
    der1[1] = (this->errorArray[0] - this->errorArray[1]) /
	    static_cast<double>(this->etimeArray[1] - this->etimeArray[0]);
    double der2 = (der1[0] - der1[1]) /
	    static_cast<double>(this->etimeArray[0]);

    // get the effect of gains
    double ctrlProp (this->gainProp * error);
    double ctrlDer1 (this->gainDer1 * der1[0]);
    double ctrlDer2 (this->gainDer2 * der2);
    this->control = -static_cast<int>(ctrlProp + ctrlDer1 + ctrlDer2);

    int controlMax = 1.5 * this->motorSpeedFollowing;
    if (this->control > controlMax)
	this->control = controlMax;
    else if (this->control < -controlMax)
	this->control = -controlMax;

    int dSpeed = this->control;

    // adjust motor speed
    if (this->motorsActive) {
	motor.speed(pins::MOTOR_PIN_L, dSpeed - this->motorSpeed);
	motor.speed(pins::MOTOR_PIN_R, dSpeed + this->motorSpeed);
    } else {
	motor.speed(pins::MOTOR_PIN_L, 0);
	motor.speed(pins::MOTOR_PIN_R, 0);
    }

    // increase time counters
    for (auto &t : etimeArray)
	++t;
}


void TapeFollow::start()
{
    // TODO: remove this junk
    LCD.clear();
    LCD.print("Starting mMI...");
    delay(1000);

    MinorMode::start();
    this->motorsActive = true;
}


void TapeFollow::pause()
{
    MinorMode::pause();
    this->motorsActive = false;
    this->printLCD();
}


void TapeFollow::test()
{
    MinorMode::test();
    this->motorsActive = false;
}


void TapeFollow::setMotorSpeedPassengerSeek()
{
    this->motorSpeedFollowing = this->motorSpeedPassengerSeek;
}


void TapeFollow::resetMotorSpeed()
{
    this->motorSpeedFollowing = this->motorSpeedFollowingDefault;
}


bool TapeFollow::isTurning()
{
    return this->turning;
}


bool TapeFollow::isSeeking()
{
    return this->seeking;
}
