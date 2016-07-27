///
// TapeFollow.cpp
//
#include <StandardCplusplus.h>
#include <phys253.h>
#include "pins.hpp"
#include "Direction.hpp"
#include "debug.hpp"
#include "TapeFollow.hpp"


const int MOTOR_SPEED_FOLLOWING      {120};
const int MOTOR_SPEED_PASSENGER_SEEK  {64};
const int MOTOR_SPEED_TURNING         {32};
const int MOTOR_SPEED_SEEKING          {8};
const int MOTOR_SPEED_REVERSE        {-12};
const float ERROR_SMALL     {.02};
const float ERROR_MEDIUM    {.04};
const float ERROR_LARGE     {.08};
const float ERROR_SEEKING   {.64};
const float ERROR_TURNING {12.80};
const float EPSILON         {.10};
const float GAIN_PROP      {4.68};
const float GAIN_DER1      {9.54};
const float GAIN_DER2     {.5*GAIN_DER1*GAIN_DER1/GAIN_PROP*(1.-EPSILON)};
// const float GAIN_DER2 {0.};
const int PRINT_PERIOD           {200};
const int NUM_SAVED_READINGS      {52};
const int INTERSECT_PERIOD         {5};  
const int TURNING_PERIOD          {10}; 
const int TURN_WAIT_PERIOD        {45};
const int OFF_TAPE_PERIOD         {50};
const int ON_TAPE_PERIOD          {10};
const int INTERSECT_DELAY_PERIOD {100};
const int TapeFollow::motorPinL         {pins::MOTOR_PIN_L};
const int TapeFollow::motorPinR         {pins::MOTOR_PIN_R};
const int *TapeFollow::tapeSensorsFront {pins::TAPE_SENSORS_FRONT};
const int *TapeFollow::tapeSensorsBack  {pins::TAPE_SENSORS_BACK};


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

    this->intersectSeen.reset();    // 00
    this->intersectDetect.reset();  // 00
    this->pinReadings.reset();      // 0000

    this->etimeArray          = {0,  1};
    this->errorArray          = {0., 0.};

    for (int i(0); i < 4; ++i) {
	// reset counters
	this->onTapeCounter[i] = 0;
	this->offTapeCounter[i] = 0;
	// assign active pins
	this->activePins[i] = TapeFollow::tapeSensorsFront[i];
    }

    // declare active pins as inputs
    for (int i(0); i < 4; ++i)
	pinMode(this->activePins[i], INPUT);
}


// TODO make this more advanced
float TapeFollow::seekTape()
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
    bool intersectSeenL = (this->mainsOnTape &&
            (this->onTapeCounter[0] >= this->intersectPeriod));
    bool intersectSeenR = (this->mainsOnTape &&
            (this->onTapeCounter[3] >= this->intersectPeriod));

    // if seen, update instance variable
    if (intersectSeenL)
	this->intersectSeen[0] = 1;
    if (intersectSeenR)
	this->intersectSeen[1] = 1;
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
	this->intersectDetect[0] = 1;
    if ((!intersectR) && this->intersectSeen[1])
	this->intersectDetect[1] = 1;

    // if intersection(s) detected, make move decision
    if ((this->onTapeCounter[0] >= this->turnWaitPeriod) &&
	(this->onTapeCounter[3] >= this->turnWaitPeriod)) {

	// wait until both intersections crossed over
	this->turnDirection = this->chooseTurn(
	        this->intersectDetect[0],
		this->intersectDetect[1],
		(mainL || mainR)
        );
	if (this->turnDirection != Direction::FRONT)
	    this->turning = true;  // activates `makeTurn` function
	// reset intersection arrays
	this->intersectSeen.reset();    // 00
	this->intersectDetect.reset();  // 00
    }
}


float TapeFollow::followTape()
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


// TODO make more advanced
float TapeFollow::makeTurn()
{
    // determine whether end has bee reached
    if ((!this->halfTurn) &&
	    (this->offTapeCounter[1] >= this->turningPeriod) &&
	    (this->offTapeCounter[2] >= this->turningPeriod)) {
    	this->halfTurn = true;
    } else if (this->halfTurn &&
            (this->onTapeCounter[1] >= this->onTapePeriod) &&
	    (this->onTapeCounter[2] >= this->onTapePeriod)) {
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
		LCD.print("<");
		break;
	    case Direction::FRONT:
		LCD.print("^");
		break;
	    case Direction::RIGHT:
		LCD.print(">");
		break;
	    case Direction::BACK:
		LCD.print("v");
		break;
	    }
    	} else {
    	    if (this->control < 0)
    		LCD.print("<");
    	    else if (this->control > 0)
    		LCD.print(">");
    	    else
    		LCD.print("^");
    	}

    	// print QRD readings
	for (int i(0); i < 4; ++i) {
	    LCD.print(" ");
	    LCD.print(this->pinReadings[i]);
	}

	// print current available RAM
	LCD.print(" ");
	LCD.print(freeRam());

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
      gainProp         (GAIN_PROP),
      gainDer1         (GAIN_DER1),
      gainDer2         (GAIN_DER2),
      errorSmall       (ERROR_SMALL),
      errorMedium      (ERROR_MEDIUM),
      errorLarge       (ERROR_LARGE),
      errorSeeking     (ERROR_SEEKING),
      errorTurning     (ERROR_TURNING),
      intersectDelay   (INTERSECT_DELAY_PERIOD),
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
      motorSpeedReverse          (MOTOR_SPEED_REVERSE),
      motorSpeedFollowing        (MOTOR_SPEED_FOLLOWING)
{
    this->init();
}


TapeFollow::~TapeFollow() {}


void TapeFollow::loop()
{
    if (!this->active)
	return;

    // LCD.clear();
    // LCD.print("MOTOR SPEEDS:");
    // delay(1000);

    // LCD.clear();
    // LCD.print("Turning:");
    // LCD.setCursor(0, 1);
    // LCD.print(this->motorSpeedTurning);
    // delay(1000);

    // LCD.clear();
    // LCD.print("Tape seeking:");
    // LCD.setCursor(0, 1);
    // LCD.print(this->motorSpeedSeeking);
    // delay(1000);

    // LCD.clear();
    // LCD.print("Tape following:");
    // LCD.setCursor(0, 1);
    // LCD.print(this->motorSpeedFollowing);
    // delay(1000);

    // LCD.clear();
    // LCD.print("Def tape following:");
    // LCD.setCursor(0, 1);
    // LCD.print(this->motorSpeedFollowingDefault);
    // delay(1000);

    // LCD.clear();
    // LCD.print("Passenger seek:");
    // LCD.setCursor(0, 1);
    // LCD.print(this->motorSpeedPassengerSeek);
    // delay(1000);

    // LCD.clear();
    // LCD.print("Reverse:");
    // LCD.setCursor(0, 1);
    // LCD.print(this->motorSpeedReverse);
    // delay(1000);

    if (this->printCount % this->printPeriod == 0) {
	this->printLCD();
	this->printCount = 0;
    }
    ++this->printCount;

    // set gains
    // TODO move this to constructor once values are decided upon
    if (!this->motorsActive) {
	this->gainProp = static_cast<float>(knob(pins::KNOB_PROP_GAIN)) / 50.;
	this->gainDer1 = static_cast<float>(knob(pins::KNOB_DER1_GAIN)) / 50.;
	this->gainDer2 = .5*this->gainDer1*this->gainDer1 /
	        this->gainProp*(1.-EPSILON);
    }

    // get readings from tape sensors
    for (auto i(0); i < 4; ++i) 
	this->pinReadings[i] = digitalRead(this->activePins[i]);
    

    // update counters
    for (int i(0); i < 4; ++i)
	if (this->pinReadings[i]) {
	    this->offTapeCounter[i] = 0;
	    if (this->onTapeCounter[i] < this->onTapePeriod)
		this->onTapeCounter[i] += 1;
	} else {
	    this->onTapeCounter[i] = 0;
	    if (this->offTapeCounter[i] < this->offTapePeriod)
		this->offTapeCounter[i] += 1;
	}

    // determine whether on tape
    this->lastOnTape = this->onTape;
    this->onTape = this->pinReadings.any();

    // determine whether mains on tape
    this->lastMainsOnTape = this->mainsOnTape;
    this->mainsOnTape = (this->pinReadings[1] || this->pinReadings[2]);

    // get error based on current state
    bool amOffTape(true);
    for (int i(0); i < 4; ++i)
	amOffTape = amOffTape && (this->offTapeCounter[i] > this->offTapePeriod);
    this->seeking = (!this->turning) && (amOffTape);
    
    float error(0.);
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
    float der1[2];
    der1[0] = (error - this->errorArray[0]) /
  	    static_cast<float>(this->etimeArray[0]);
    der1[1] = (this->errorArray[0] - this->errorArray[1]) /
	    static_cast<float>(this->etimeArray[1] - this->etimeArray[0]);
    float der2 = (der1[0] - der1[1]) /
	    static_cast<float>(this->etimeArray[0]);

    // get the effect of gains
    float ctrlProp (this->gainProp * error);
    float ctrlDer1 (this->gainDer1 * der1[0]);
    float ctrlDer2 (this->gainDer2 * der2);
    this->control = -static_cast<int>(ctrlProp + ctrlDer1 + ctrlDer2);

    // LCD.clear();
    // LCD.print(this->motorSpeedFollowing);
    // LCD.setCursor(0, 1);
    // LCD.print(this->control);

    int controlMax = this->motorSpeedFollowing * 3 / 2;
    if (this->control > controlMax)
	this->control = controlMax;
    else if (this->control < -controlMax)
	this->control = -controlMax;

    // LCD.print(" ");
    // LCD.print(this->control);
    // delay(100);

    int dSpeed = this->control;

    // adjust motor speed
    if (this->motorsActive) {
	motor.speed(TapeFollow::motorPinL, dSpeed - this->motorSpeed);
	motor.speed(TapeFollow::motorPinR, dSpeed + this->motorSpeed);
    } else {
	motor.speed(TapeFollow::motorPinL, 0);
	motor.speed(TapeFollow::motorPinR, 0);
    }

    // increase time counters
    for (auto &t : etimeArray)
	++t;
}


void TapeFollow::start()
{
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


// TODO
void TapeFollow::turnAround()
{
    // Reverse robot
    this->motorSpeed = this->motorSpeedReverse;

    // Set `turnDirection` and call `makeTurn()`
    this->turnDirection = Direction::BACK;
    this->turning = true;
}


bool TapeFollow::isTurning()
{
    return this->turning;
}


bool TapeFollow::isSeeking()
{
    return this->seeking;
}
