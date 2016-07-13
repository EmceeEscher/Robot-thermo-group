///
// TapeFollow3.cpp
//
#include "TapeFollow3.hpp"


const int TAPE_SENSORS_FRONT[]  {0, 1, 2, 3};
const int TAPE_SENSORS_BACK[]   {4, 5, 6, 7};
const int MOTOR_PIN_L(0);
const int MOTOR_PIN_R(3);
const int KNOB_PROP_GAIN(6);
const int KNOB_DER1_GAIN(7);
const float EPSILON(0.01);
const int MOTOR_SPEED(100);
const int PRINT_PERIOD(96);
const double ERROR_SMALL(2);
const double ERROR_MEDIUM(4);
const double ERROR_LARGE(8);
const double ERROR_TURNING(8);


void TapeFollow3::init()
{
    this->active = false;
    this->onTape = false;
    this->turning = false;
    this->halfTurn = false;
    this->error = 0.;
    this->lastError = 0.;
    this->turnDirection = 0;
    this->motorSpeed = MOTOR_SPEED;

    // set errors

    for (int i(0); i < 2; ++i) {
	this->errorArray[i] = 0;
	this->etimeArray[i] = i;
	this->intersectSeen[i] = false;
	this->intersectDetect[i] = false;
    }

    for (int i(0); i < 4; ++i) {
	this->activePins[i] = TAPE_SENSORS_FRONT[i];
	this->lastPinReadings[i] = false;
	this->pinReadings[i] = false;
    }
}


// TODO make this more advanced
void TapeFollow3::seekTape()
{
    if (this->lastError < 0.)              // off tape to the right
	this->error = -this->errorLarge;
    else if (this->lastError > 0.)         // off tape to the left
	this->error = this->errorLarge;
    else
	this->error = 0.;
}


void TapeFollow3::followTape()
{
    // declare static variables (runs once)
    const static bool &intersectL = this->pinReadings[0];
    const static bool &mainL      = this->pinReadings[1];
    const static bool &mainR      = this->pinReadings[2];
    const static bool &intersectR = this->pinReadings[3];

    // determine error
    if (mainL && mainR)                    // both pins over tape
	this->error = 0.;
    else if (mainL)                       // left main over tape
	this->error = -this->errorSmall;
    else if (mainR)                       // right main over tape
	this->error = this->errorSmall;
    else if (intersectL && (!intersectR))  // left intersection over tape
	this->error = -this->errorMedium;
    else if (intersectR && (!intersectL))  // right intersection over tape
	this->error = this->errorMedium;
    else
	this->error = 0.;

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


// TODO make more advanced
void TapeFollow3::makeTurn()
{
    // declare static variables (runs once)
    static bool mainsOnTape;

    mainsOnTape = (this->pinReadings[1] || this->pinReadings[2]);

    // determine error
    if (mainsOnTape)
	this->error = 0.;
    else 
	this->error = this->turnDirection * this->errorTurning;

    // determine whether end has bee reached
    if ((!this->halfTurn) && this->lastOnTape && (!mainsOnTape))
	this->halfTurn = true;
    else if (this->halfTurn && (!this->lastOnTape) && mainsOnTape) {
	this->halfTurn = false;
	this->turning = false;  // exit to regular following
    }
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
    static int i(0);  // declaration runs once
    if (i % PRINT_PERIOD == 0)
	i = 0;
    else if (!this->active) {
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
	LCD.print(" ");
	LCD.print(this->gainProp);
	LCD.print(" ");
	LCD.print(this->gainDer1);
	LCD.print(" ");
	LCD.print(this->control);
    }
    ++i;
}


TapeFollow3::TapeFollow3()
    : errorSmall(ERROR_SMALL),
      errorMedium(ERROR_MEDIUM),
      errorLarge(ERROR_LARGE),
      errorTurning(ERROR_TURNING)
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

    if (!this->active)
	return;

    this->printLCD();

    // set gains
    // TODO move this to `init` once values are decided upon
    this->gainProp = knob(KNOB_PROP_GAIN) / 50;
    this->gainDer1 = knob(KNOB_DER1_GAIN) / 50;
    this->gainDer2 = .5*this->gainDer1*this->gainDer1/this->gainProp*(1.-EPSILON);

    // get readings from tape sensors
    for (int i(0); i < 4; ++i) {
	this->lastPinReadings[i] = this->pinReadings[i];
	this->pinReadings[i] = static_cast<bool>(
	        digitalRead(this->activePins[i]));
    }
    this->lastOnTape = this->onTape;
    this->onTape = false;
    for (int i(0); i < 4; ++i)
	this->onTape = this->onTape || this->pinReadings[i];

    // get error based on current state
    if (!(this->turning || this->onTape))
	this->seekTape();
    else if (this->turning)
	this->makeTurn();
    else
	this->followTape();

    // update previous error parameters
    if (this->error != this->lastError) {
	this->errorArray[1] = this->errorArray[0];
	this->errorArray[0] = this->lastError;
	this->etimeArray[1] = this->etimeArray[0];
	this->etimeArray[0] = 1;
	this->lastError = this->error;
    }

    // get error derivatives
    der1[0] = (this->error - this->errorArray[0]) /
	    static_cast<double>(this->etimeArray[0]);
    der1[1] = (this->errorArray[0] - this->errorArray[1]) /
	    static_cast<double>(this->etimeArray[1] - this->etimeArray[0]);
    der2 = (der1[0] - der1[1]) /
	    static_cast<double>(this->etimeArray[0]);

    // get the effect of gains
    ctrlProp = this->gainProp * this->error;
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
