///
// TapeFollow3.cpp
//
#include <StandardCplusplus.h>
#include <vector>
#include <phys253.h>
#include "TapeFollow3.hpp"


using std::vector;


const int TAPE_SENSORS_FRONT[]  {0, 1, 2, 3};
const int TAPE_SENSORS_BACK[]   {4, 5, 6, 7};
const int MOTOR_PIN_L(0);
const int MOTOR_PIN_R(3);
const int KNOB_PROP_GAIN(6);
const int KNOB_DER1_GAIN(7);
const double EPSILON(0.01);
const int MOTOR_SPEED(72);
const int PRINT_PERIOD(16);
const unsigned long INTERSECT_DELAY_PERIOD(10000);
const double ERROR_SMALL(2);
const double ERROR_MEDIUM(4);
const double ERROR_LARGE(8);
const double ERROR_TURNING(8);
const double GAIN_PROP(5.);
const double GAIN_DER1(5.);
// const double GAIN_DER2(0.);
const double GAIN_DER2(.5*GAIN_DER1*GAIN_DER1/GAIN_PROP*(1.-EPSILON));
const int NUM_SAVED_READINGS(4);


void TapeFollow3::init()
{
    this->active = false;
    this->onTape = false;
    this->lastOnTape = false;
    this->mainsOnTape = false;
    this->lastMainsOnTape = false;
    this->turning = false;
    this->halfTurn = false;
    this->motorsActive = false;

    this->turnDirection = 0;
    this->control = 0;
    this->printCount = 0;
    this->motorSpeed = MOTOR_SPEED;
    this->tapeFollowSteps = 0;

    this->lastError = 0.;

    for (int i(0); i < 2; ++i) {
	this->intersectSeen[i] = false;
	this->intersectDetect[i] = false;
	this->etimeArray[i] = i;
	this->errorArray[i] = 0.;
    }

    for (int i(0); i < 4; ++i) {
	this->activePins[i] = TAPE_SENSORS_FRONT[i];
	this->pinReadings[i] = false;
    }

    for (unsigned int i(0); i < this->lastPinReadings.size(); ++i) 
    	for (int j(0); j < 4; ++j)
    	    this->lastPinReadings[i][j] = false;
}


// TODO make this more advanced
double TapeFollow3::seekTape()
{
    if (this->lastError < 0.)              // off tape to the right
	return this->errorLarge;
    else if (this->lastError > 0.)         // off tape to the left
	return -this->errorLarge;
    else
	return 0.;
}


void TapeFollow3::intersectionDetection()
{
    // declare static variables (runs once)
    const static bool &intersectL = this->pinReadings[0];
    const static bool &mainL      = this->pinReadings[1];
    const static bool &mainR      = this->pinReadings[2];
    const static bool &intersectR = this->pinReadings[3];

    // check if intersections seen
    if (intersectL && mainR)
	this->intersectSeen[0] = true;
    if (intersectR && mainL)
	this->intersectSeen[1] = true;

    // check if intersection detected
    if ((!intersectL) && this->intersectSeen[0])
	this->intersectDetect[0] = true;
    if ((!intersectR) && this->intersectSeen[1])
	this->intersectDetect[1] = true;

    // if intersection(s) detected, make move decision
    if (!(intersectL || intersectR)) {  // wait until both intersections crossed over
	this->turnDirection = this->chooseTurn(
	        this->intersectDetect[0],
		this->intersectDetect[1],
		(mainL || mainR)
        );
	if (this->turnDirection != 0)
	    this->turning = true;  // activates `makeTurn` function
	// reset intersection arrays
	for (int i(0); i < 2; ++i) {
	    this->intersectSeen[i] = false;
	    this->intersectDetect[i] = false;
	}
    }
}


double TapeFollow3::followTape()
{
    // declare static variables (runs once)
    const static bool &intersectL = this->pinReadings[0];
    const static bool &mainL      = this->pinReadings[1];
    const static bool &mainR      = this->pinReadings[2];
    const static bool &intersectR = this->pinReadings[3];
    static double error = 0.;

    // determine error
    if (mainL && mainR)                    // both pins over tape
	error = 0.;
    else if (mainL)                       // left main over tape
	error = this->errorSmall;
    else if (mainR)                       // right main over tape
	error = -this->errorSmall;
    else if (intersectL && (!intersectR))  // left intersection over tape
	error = this->errorMedium;
    else if (intersectR && (!intersectL))  // right intersection over tape
	error = -this->errorMedium;
    else
	error = 0.;

    if (this->tapeFollowSteps > this->intersectDelay)
	this->intersectionDetection();

    return error;
}


// TODO make more advanced
double TapeFollow3::makeTurn()
{
    static double error(0.);

    // determine error
    if (this->mainsOnTape)
	error = 0.;
    else 
	error = -this->turnDirection * this->errorTurning;

    // determine whether end has bee reached
    if ((!this->halfTurn) && this->lastMainsOnTape && (!this->mainsOnTape))
	this->halfTurn = true;
    else if (this->halfTurn && (!this->lastMainsOnTape) && this->mainsOnTape) {
	this->halfTurn = false;
	this->turning = false;  // exit to regular following
    }

    return error;
}


// TODO: generalize
int TapeFollow3::chooseTurn(bool left, bool right, bool straight)
{
    // for now, prefer right, then left, then straight
    if (right)
	return 1;  // right
    else if (left)
	return -1; // left
    else
	return 0;  // straight
}


void TapeFollow3::printLCD()
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
	    if (this->turnDirection == -1)
		LCD.print("<--");
	    else if (this->turnDirection == 1)
		LCD.print("-->");
	    else
		LCD.print(" ^ ");
	} else {
	    if (this->control < 0)
		LCD.print("<  ");
	    else if (this->control > 0)
		LCD.print("  >");
	    else
		LCD.print(" ^ ");
	}
	// print QRD readings
	for (int j(0); j < 4; ++j) {
	    LCD.print(" ");
	    LCD.print(this->pinReadings[j]);
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


TapeFollow3::TapeFollow3()
    : gainProp        (GAIN_PROP),
      gainDer1        (GAIN_DER1),
      gainDer2        (GAIN_DER2),
      errorSmall      (ERROR_SMALL),
      errorMedium     (ERROR_MEDIUM),
      errorLarge      (ERROR_LARGE),
      errorTurning    (ERROR_TURNING),
      intersectDelay  (INTERSECT_DELAY_PERIOD),
      printPeriod     (PRINT_PERIOD),
      pinReadings     (4, false),
      lastPinReadings (NUM_SAVED_READINGS, vector<bool>(4, false))
{
    this->init();
}


void TapeFollow3::loop()
{
    // declare static variables (runs only once)
    static double ctrlProp;
    static double ctrlDer1;
    static double ctrlDer2;
    static double der1[2];
    static double der2;
    static double error(0.);

    if (!this->active)
	return;

    if (this->printCount % this->printPeriod == 0) {
	this->printLCD();
	this->printCount = 1;
    } else
	++this->printCount;

    // // set gains
    // // TODO move this to constructor once values are decided upon
    // this->gainProp = GAIN_PROP;//static_cast<double>(knob(KNOB_PROP_GAIN)) / 50.;
    // this->gainDer1 = GAIN_DER1;//static_cast<double>(knob(KNOB_DER1_GAIN)) / 50.;
    // this->gainDer2 = 0.; //.5*this->gainDer1*this->gainDer1/this->gainProp*(1.-EPSILON);

    // get readings from tape sensors
    for (int i(0); i < 4; ++i) {
	this->pinReadings[i] = static_cast<bool>(
	        digitalRead(this->activePins[i]));
    }

    // TODO update lastPinReadings array
    this->lastPinReadings.pop_back();
    this->lastPinReadings.push_front(this->pinReadings);
    
    this->lastOnTape = this->onTape;
    this->onTape = false;
    for (int i(0); i < 4; ++i)
	this->onTape = this->onTape || this->pinReadings[i];
    this->lastMainsOnTape = this->mainsOnTape;
    this->mainsOnTape = (this->pinReadings[1] || this->pinReadings[2]);

    // get error based on current state
    if (!(this->turning || this->onTape)) {
	this->tapeFollowSteps = 0;
	error = this->seekTape();
    } else if (this->turning) {
	this->tapeFollowSteps = 0;
	error = makeTurn();
    } else {
	this->tapeFollowSteps += 1;
	error = followTape();
    }

    // update previous error parameters
    if (error != this->lastError) {
	this->errorArray[1] = this->errorArray[0];
	this->errorArray[0] = this->lastError;
	this->etimeArray[1] = this->etimeArray[0];
	this->etimeArray[0] = 1;
	this->lastError = error;
    }

    // get error derivatives
    der1[0] = (error - this->errorArray[0]) /
	    static_cast<double>(this->etimeArray[0]);
    der1[1] = (this->errorArray[0] - this->errorArray[1]) /
	    static_cast<double>(this->etimeArray[1] - this->etimeArray[0]);
    der2 = (der1[0] - der1[1]) /
	    static_cast<double>(this->etimeArray[0]);

    // get the effect of gains
    ctrlProp = this->gainProp * error;
    ctrlDer1 = this->gainDer1 * der1[0];
    ctrlDer2 = this->gainDer2 * der2;
    this->control = -static_cast<int>(ctrlProp + ctrlDer1 + ctrlDer2);

    // adjust motor speed
    if (this->motorsActive) {
	motor.speed(MOTOR_PIN_L, -this->motorSpeed + this->control);
	motor.speed(MOTOR_PIN_R, this->motorSpeed + this->control);
    } else {
	motor.speed(MOTOR_PIN_L, 0);
	motor.speed(MOTOR_PIN_R, 0);
    }

    // increase time counters
    for (int i(0); i < 2; ++i)
	this->etimeArray[i] += 1;
}


void TapeFollow3::start()
{
    this->active = true;
    this->motorsActive = true;
}


void TapeFollow3::stop()
{
    this->init();
    this->pause();
}


void TapeFollow3::pause()
{
    this->active = false;
    this->motorsActive = false;
    this->printLCD();
}


bool TapeFollow3::isActive()
{
    return this->active;
}


void TapeFollow3::test()
{
    this->active = true;
    this->motorsActive = false;
}
