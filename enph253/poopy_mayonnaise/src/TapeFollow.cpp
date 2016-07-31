///
// TapeFollow.cpp
//
#include <phys253.h>
#include "pins.hpp"
#include "Direction.hpp"
#include "debug.hpp"
#include "TapeFollow.hpp"


// const int MOTOR_SPEED_FOLLOWING      {120};
const int MOTOR_SPEED_FOLLOWING       {84};
const int MOTOR_SPEED_PASSENGER_SEEK  {64};
const int MOTOR_SPEED_TURNING         {32};
const int MOTOR_SPEED_TURNING_AROUND  {-8};
// const int MOTOR_SPEED_SEEKING          {8};
const int MOTOR_SPEED_SEEKING         {32};
const int MOTOR_SPEED_REVERSE        {-64};
const float ERROR_SMALL     {.02};
const float ERROR_MEDIUM    {.04};
const float ERROR_LARGE     {.08};
const float ERROR_SEEKING   {.64};
const float ERROR_TURNING {12.80};
const float EPSILON         {.10};
const float GAIN_PROP      {9.84};
const float GAIN_DER1     {11.78};
const float GAIN_DER2     {.5*GAIN_DER1*GAIN_DER1/GAIN_PROP*(1.-EPSILON)};
// const float GAIN_DER2 {0.};
const int PRINT_PERIOD                 {200};
const int COUNTER_MAX                  {256};
const int INTERSECT_DETECT_PERIOD       {15};  
const int TURN_CONFIRM_PERIOD           {10}; 
const int PRE_TURN_DELAY_PERIOD         {75};
const int PRE_TURN_AROUND_DELAY_PERIOD {145};
const int OFF_TAPE_PERIOD               {50};
// const int ON_TAPE_PERIOD               {10};
const int ON_TAPE_PERIOD                {5};
const int INTERSECT_SEEK_DELAY_PERIOD {100};
const unsigned long RANDOM_MAX_VAL     {100000};
const int TapeFollow::motorPinL         {pins::MOTOR_PIN_L};
const int TapeFollow::motorPinR         {pins::MOTOR_PIN_R};
const int *TapeFollow::tapeSensorsFront {pins::TAPE_SENSORS_FRONT};
const int *TapeFollow::tapeSensorsBack  {pins::TAPE_SENSORS_BACK};
const TFAction TapeFollow::allActions[TapeFollow::numActions] = {
    TFAction::FOLLOWING,
    TFAction::REVERSING,
    TFAction::SEEKING,
    TFAction::TURNING
};


void TapeFollow::init()
{
    MinorMode::init();
    
    this->turningAround       = false;
    this->willTurnAround      = false;
    this->halfTurn            = false;
    this->motorsActive        = false;

    this->action              = TFAction::FOLLOWING;

    this->turnDirection       = Direction::FRONT;  // not turning
    this->control             = 0;
    this->printCount          = 0;
    this->motorSpeedFollowing = this->motorSpeedFollowingDefault;
    this->motorSpeedTurning   = this->motorSpeedTurningDefault;
    this->motorSpeed          = 0;

    this->error               = 0.;
    this->lastError           = 0.;

    this->intersectSeen.reset();    // 00
    this->intersectDetect.reset();  // 00
    this->pinReadings.reset();      // 0000

    this->etimeArray          = {0,  1};
    this->errorArray          = {0., 0.};

    for (int i(0); i < TapeFollow::numSensors; ++i) {
	// reset counters
	this->onTapeCounter[i] = 0;
	this->offTapeCounter[i] = 0;
	// assign active pins
	this->activePins[i] = TapeFollow::tapeSensorsFront[i];
    }

    for (int i(0); i < TapeFollow::numActions; ++i)
	this->steps[i] = 0;

    this->leftWeight = 0.;
    this->rightWeight = 0.;
    this->straightWeight = 0.;

    // declare active pins as inputs
    for (int i(0); i < TapeFollow::numSensors; ++i)
	pinMode(this->activePins[i], INPUT);
}


bool TapeFollow::offTape()
{
    for (int i(0); i < TapeFollow::numSensors; ++i)
	if (this->offTapeCounter[i] < this->offTapePeriod)
	    return false;
    return true;
}


bool TapeFollow::mainsOffTape()
{
    for (int i(0); i < 2; ++i)
	if (this->offTapeCounter[i+1] < this->offTapePeriod)
	    return false;
    return true;
}


// TODO
void TapeFollow::updateIntersectionsSeen()
{
    bool intSeenL = (!this->mainsOffTape() &&
            (this->onTapeCounter[0] >= this->intersectDetectPeriod));
    bool intSeenR = (!this->mainsOffTape() &&
            (this->onTapeCounter[3] >= this->intersectDetectPeriod));

    // if seen, update instance variable
    if (intSeenL)
	this->intersectSeen[0] = 1;
    if (intSeenR)
	this->intersectSeen[1] = 1;
}


void TapeFollow::updateIntersectionsDetected()
{
    // check if intersections seen
    this->updateIntersectionsSeen();

    // check if intersection detected
    for (int i(0); i < 2; ++i)
	this->intersectDetect[i] = (!this->pinReadings[i]) &&
	        this->intersectSeen[i];

    // if intersection(s) detected, make move decision
    if ((this->offTapeCounter[0] >= this->preTurnDelayPeriod) &&
	(this->offTapeCounter[3] >= this->preTurnDelayPeriod)) {

	// TODO: do this a better way
	this->turnDirection = this->chooseTurnDeterministic(  // TODO: specify this function from major mode
	        this->intersectDetect[0],
		this->intersectDetect[1],
		!this->mainsOffTape()
        );
	if (this->turnDirection != Direction::FRONT)  // TODO: move to updateSTate?
	    this->action = TFAction::TURNING;

	// reset intersection arrays
	this->intersectSeen.reset();    // 00
	this->intersectDetect.reset();  // 00
    }
}


Direction TapeFollow::chooseTurnDeterministic(bool left, bool right, bool straight)
{ 
    int prefLeft = left * (
            (this->leftWeight > this->rightWeight) +
    	    (this->leftWeight > this->straightWeight));
    int prefRight = right * (
            (this->rightWeight > this->leftWeight) +
    	    (this->rightWeight > this->straightWeight));
    int prefStraight = straight * (
            (this->straightWeight > this->leftWeight) +
    	    (this->straightWeight > this->rightWeight));

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
	    left     * this->leftWeight +
	    right    * this->rightWeight +
	    straight * this->straightWeight
    );
    float leftProb;
    float rightProb;

    if (total == 0) {
	leftProb  = left  / (left + right + straight);
	rightProb = right / (left + right + straight);
    } else {
	leftProb  = left  * this->leftWeight  / total;
	rightProb = right * this->rightWeight / total;
    }

    // TODO: do this randValue part differently?
    float randValue = static_cast<float>(random(RANDOM_MAX_VAL)) /
	(RANDOM_MAX_VAL+1);
    float leftMax = 0 + leftProb;
    float rightMax = leftProb + rightProb;
    
    this->leftWeight = 0.;
    this->rightWeight = 0.;
    this->straightWeight = 0.;

    if (randValue < leftMax) 
	return Direction::LEFT;
    else if (randValue < rightMax) 
	return Direction::RIGHT;
    else 
	return Direction::FRONT;
}


// TODO: make this more advanced
void TapeFollow::setErrorSeekTape()
{
    if (this->lastError < 0.)              // off tape to the right
	this->error = -this->errorSeeking;
    else if (this->lastError > 0.)         // off tape to the left
	this->error = this->errorSeeking;
    else
	this->error = 0.;
}


void TapeFollow::setErrorFollowTape()
{
    // Define aliases for the readings
    bool intrL = this->pinReadings[0];
    bool mainL = this->pinReadings[1];
    bool mainR = this->pinReadings[2];
    bool intrR = this->pinReadings[3];
    
    // determine error
    if (mainL && mainR)               // both main pins over tape
	this->error = 0.;
    else if (mainL)                  // left main over tape
	this->error = this->errorSmall;
    else if (mainR)                  // right main over tape
	this->error = -this->errorSmall;
    else if (intrL && (!intrR))       // left intersection over tape
	this->error = this->errorMedium;
    else if (intrR && (!intrL))       // right intersection over tape
	this->error = -this->errorMedium;
    else if (this->lastError < 0.)    // off tape to the right
	this->error = -this->errorLarge;
    else if (this->lastError > 0.)    // off tape to the left
	this->error = this->errorLarge;
    else 
	this->error = 0.;
}


// TODO: is this correct?
void TapeFollow::setErrorMakeTurn()
{
    this->error = -(static_cast<int>(this->turnDirection)-1) * this->errorTurning;
}


void TapeFollow::setError()
{
    // choose error based on current action
    switch (this->action) {  // TODO: handle all cases
    case TFAction::SEEKING:
	this->setErrorSeekTape();
	break;
    case TFAction::TURNING:
	this->setErrorMakeTurn();
	break;
    default:
	this->setErrorFollowTape();
	break;
    }
    this->error *= std::abs(this->motorSpeedFollowing);  // TODO

    // update previous error parameters
    if (this->error != this->lastError) {
	this->errorArray = {this->lastError, this->errorArray[0]};
	this->etimeArray = {0,              this->etimeArray[0]};
	this->lastError  = this->error;
    }

    // increase time counters
    for (auto &t : etimeArray)
	++t;
}


void TapeFollow::setControl()
{
    // get error derivatives
    float der1[2];
    der1[0] = (this->error - this->errorArray[0]) /
  	    static_cast<float>(this->etimeArray[0]);
    der1[1] = (this->errorArray[0] - this->errorArray[1]) /
	    static_cast<float>(this->etimeArray[1] - this->etimeArray[0]);
    float der2 = (der1[0] - der1[1]) /
	    static_cast<float>(this->etimeArray[0]);

    // get the effect of gains
    float ctrlProp (this->gainProp * this->error);
    float ctrlDer1 (this->gainDer1 * der1[0]);
    float ctrlDer2 (this->gainDer2 * der2);
    this->control = -static_cast<int>(ctrlProp + ctrlDer1 + ctrlDer2);

    int controlMax = std::abs(this->motorSpeedFollowing) * 3 / 2;  // TODO
    if (this->control > controlMax)
	this->control = controlMax;
    else if (this->control < -controlMax)
	this->control = -controlMax;
}


// TODO: !!! Determine how to modify/respond to action
// TODO: !!! Serious cleanup needed !
void TapeFollow::updateState()
{
    int followSteps  = this->steps[static_cast<int>(TFAction::FOLLOWING)];
    int reverseSteps = this->steps[static_cast<int>(TFAction::REVERSING)];

    switch (this->action) {

    case TFAction::FOLLOWING:
	this->motorSpeed = this->motorSpeedFollowing;
	if (this->offTape())
	    this->action = TFAction::SEEKING;
	else if (followSteps >= this->intersectSeekDelayPeriod)
	    this->updateIntersectionsDetected();
	break;

    case TFAction::REVERSING:
	this->motorSpeed = this->motorSpeedReverse;
	if (this->offTape())
	    this->action = TFAction::SEEKING;
	else if (this->willTurnAround) {
	    if (reverseSteps >= this->preTurnAroundDelayPeriod) {
		this->motorSpeedFollowing = this->motorSpeedFollowingDefault;
		this->motorSpeedTurning = this->motorSpeedTurningAround;
		this->willTurnAround = false;
		this->turningAround = true;
		this->action = TFAction::TURNING;
	    }
	}
	break;

    case TFAction::SEEKING:
	this->motorSpeed = this->motorSpeedSeeking;
	if (!this->offTape()) {
	    if (!this->willTurnAround)
		this->action = TFAction::FOLLOWING;
	    else
		this->action = TFAction::REVERSING;
	}
	break;

    case TFAction::TURNING:
	this->motorSpeed = this->motorSpeedTurning;
	if ((!this->halfTurn) &&
	    (this->offTapeCounter[1] >= this->turnConfirmPeriod) &&
	    (this->offTapeCounter[2] >= this->turnConfirmPeriod)) {
	    this->halfTurn = true;
	} else if (
		   this->halfTurn &&
		   ((this->onTapeCounter[1] >= this->onTapePeriod) ||
		    (this->onTapeCounter[2] >= this->onTapePeriod))) {
	    this->willTurnAround = false;
	    this->halfTurn = false;
	    this->action = TFAction::FOLLOWING; // exit to regular following
	    this->turningAround = false;
	    this->turnDirection = Direction::FRONT;
	    this->motorSpeedTurning = this->motorSpeedTurningDefault;
	    this->motorSpeedFollowing = this->motorSpeedFollowingDefault;
	}
	break;

    }
}


void TapeFollow::updateCounters()
{
    // update steps counters
    for (int i(0); i < TapeFollow::numActions; ++i) {
	if (this->action != TapeFollow::allActions[i]) 
	    this->steps[i] = 0;
	else if (this->steps[i] < this->counterMax)
	    this->steps[i] += 1;
    }
    
    // update on/off tape counters
    for (int i(0); i < TapeFollow::numSensors; ++i) {
	if (this->pinReadings[i]) {
	    this->offTapeCounter[i] = 0;
	    if (this->onTapeCounter[i] < this->counterMax)
		this->onTapeCounter[i] += 1;
	} else {
	    this->onTapeCounter[i] = 0;
	    if (this->offTapeCounter[i] < this->counterMax)
		this->offTapeCounter[i] += 1;
	}
    }
}


// TODO: !!! move this outside of this class
void TapeFollow::printLCD()
{
    LCD.clear();
    // print letter
    switch (this->action) {
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
    if (this->turningAround)
	LCD.print( F("v") );
    else if (this->action == TFAction::TURNING) {
	switch (this->turnDirection) {
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
	if (this->control < 0)
	    LCD.print( F("<") );
	else if (this->control > 0)
	    LCD.print( F(">") );
	else
	    LCD.print( F("^") );
    }
    
    // print QRD readings
    for (int i(0); i < TapeFollow::numSensors; ++i) {
	LCD.print( F(" ") );
	LCD.print(this->pinReadings[i]);
    }

    // print current available RAM
    LCD.print( F(" ") );
    LCD.print(freeRam());
    
    // print gains and control
    LCD.setCursor(0,1);
    LCD.print(this->gainProp);
    // LCD.print(this->errorArray[0]);
    LCD.print( F(" ") );
    LCD.print(this->gainDer1);
    // LCD.print(this->errorArray[1]);
    LCD.print( F(" ") );
    LCD.print(this->control);
}


TapeFollow::TapeFollow()
    : MinorMode(),
      gainProp                   (GAIN_PROP),
      gainDer1                   (GAIN_DER1),
      gainDer2                   (GAIN_DER2),
      errorSmall                 (ERROR_SMALL),
      errorMedium                (ERROR_MEDIUM),
      errorLarge                 (ERROR_LARGE),
      errorSeeking               (ERROR_SEEKING),
      errorTurning               (ERROR_TURNING),
      intersectSeekDelayPeriod   (INTERSECT_SEEK_DELAY_PERIOD),
      intersectDetectPeriod      (INTERSECT_DETECT_PERIOD),
      turnConfirmPeriod          (TURN_CONFIRM_PERIOD),
      preTurnDelayPeriod         (PRE_TURN_DELAY_PERIOD),
      preTurnAroundDelayPeriod   (PRE_TURN_AROUND_DELAY_PERIOD),
      offTapePeriod              (OFF_TAPE_PERIOD),
      onTapePeriod               (ON_TAPE_PERIOD),
      printPeriod                (PRINT_PERIOD),
      counterMax                 (COUNTER_MAX),
      motorSpeedTurningDefault   (MOTOR_SPEED_TURNING),
      motorSpeedTurningAround    (MOTOR_SPEED_TURNING_AROUND),
      motorSpeedSeeking          (MOTOR_SPEED_SEEKING),
      motorSpeedFollowingDefault (MOTOR_SPEED_FOLLOWING),
      motorSpeedPassengerSeek    (MOTOR_SPEED_PASSENGER_SEEK),
      motorSpeedReverse          (MOTOR_SPEED_REVERSE),
      motorSpeedFollowing        (MOTOR_SPEED_FOLLOWING),
      motorSpeedTurning          (MOTOR_SPEED_TURNING)
{
    this->init();
}


TapeFollow::~TapeFollow() {}


void TapeFollow::loop()
{
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
    for (int i(0); i < TapeFollow::numSensors; ++i) 
	this->pinReadings[i] = digitalRead(this->activePins[i]);

    // update counters
    this->updateCounters();
    
    // update state variables
    this->updateState();

    // get error based on current state
    this->setError();

    // get the control to apply to the motor speed
    this->setControl();

    // adjust motor speed
    if (this->motorsActive) {
	motor.speed(TapeFollow::motorPinL, this->control - this->motorSpeed);
	motor.speed(TapeFollow::motorPinR, this->control + this->motorSpeed);
    } else {
	motor.speed(TapeFollow::motorPinL, 0);
	motor.speed(TapeFollow::motorPinR, 0);
    }

}


void TapeFollow::start()
{
    MinorMode::start();
    this->motorsActive = true;
}


void TapeFollow::pause()
{
    MinorMode::pause();
    motor.speed(TapeFollow::motorPinL, 0);
    motor.speed(TapeFollow::motorPinR, 0);
    this->motorsActive = false;
    this->printLCD();
}


void TapeFollow::test()
{
    MinorMode::test();
    this->pause();
}


// TODO: !!! find a better way
void TapeFollow::setMotorSpeedPassengerSeek()
{
    this->motorSpeedFollowing = this->motorSpeedPassengerSeek;
}


// TODO: !!! find a better way
void TapeFollow::resetMotorSpeed()
{
    this->motorSpeedFollowing = this->motorSpeedFollowingDefault;
}


// TODO: !!! check if this is correct
void TapeFollow::turnAround()
{
    switch (this->turnDirection) {
    case Direction::LEFT:
	this->turnDirection = Direction::RIGHT;
	break;
    case Direction::RIGHT:
	this->turnDirection = Direction::LEFT;
	break;
    case Direction::FRONT:
	this->turnDirection = Direction::RIGHT;  // TODO: make a smarter way of choosing this
	break;
    case Direction::BACK:
	this->turnDirection = Direction::RIGHT;  // TODO: make a smarter way of choosing this
	break;
    }
    this->willTurnAround = true;
    this->action = TFAction::SEEKING;
}


bool TapeFollow::isTurning()
{
    return this->action == TFAction::TURNING;
}


bool TapeFollow::isSeeking()
{
    return this->action == TFAction::SEEKING;
}


void TapeFollow::giveTurnDirection(float left, float right, float straight){
    this->leftWeight = left;
    this->rightWeight = right;
    this->straightWeight = straight;
}
