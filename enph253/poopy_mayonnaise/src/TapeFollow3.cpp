///
// TapeFollow3.cpp
//
#include "TapeFollow3.hpp"


const int MOTOR_PIN_L(0);
const int MOTOR_PIN_R(3);
const int KNOB_PROP_GAIN(6);
const int KNOB_DER1_GAIN(7);
const float EPSILON(0.01);
const int MOTOR_SPEED(100);


void TapeFollow3::init()
{
    this->active = false;
    this->onTape = false;
    this->turning = false;
    this->lastError = 0;
    this->turnDirection = 0;
    this->motorSpeed = MOTOR_SPEED;
    // set gains
    this->gainProp = knob(KNOB_PROP_GAIN) / 50;
    this->gainDer1 = knob(KNOB_DER1_GAIN) / 50;
    this->gainDer2 = .5*this->gainDer1*this->gainDer1/this->gainProp*(1.-EPSILON);
    // set errors
    for (int i(0); i < 2; ++i) {
	this->errorArray[i] = 0;  // {0, 0}
	this->etimeArray[i] = i;  // {0, 1}
    }
}

void TapeFollow3::seekTape()
{
    // TODO
}

// TODO
void TapeFollow3::followTape()
{
    // declare static variables (runs once)
    static double ctrlProp;
    static double ctrlDer1;
    static double ctrlDer2;
    static double der1[2];
    static double der2;
    static int control;
    static double error;
    static bool pinReadings[4];
    const static bool &intersectL = pinReadings[0];
    const static bool &mainL      = pinReadings[1];
    const static bool &mainR      = pinReadings[2];
    const static bool &intersectR = pinReadings[3];

    // get readings from tape sensors
    for (int i(0); i < 4; ++i)
	pinReadings[i] = static_cast<bool>(digitalRead(this->activePins[i]));

    // determine error
    if (mainL && mainR)                    // both pins over tape
	error = 0.;
    else if (mainL)                       // left main over tape
	error = -1.; // TODO stub
    else if (mainR)                       // right main over tape
	error = 1.;  // TODO stub
    else if (intersectL && (!intersectR))  // left intersection over tape
	error = -2.; // TODO stub
    else if (intersectR && (!intersectL))  // right intersection over tape
	error = 2.;  // TODO stub
    else if (this->lastError < 0.)         // off tape to the right
	error = -3.; // TODO stub
    else if (this->lastError > 0.)         // off tape to the left
	error = 3.;  // TODO stub
    else
	error = 0.;

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
	static_cast<double>(etimeArray[0]);
    der1[1] = (this->errorArray[0] - this->errorArray[1]) /
	static_cast<double>(etimeArray[1] - etimeArray[0]);
    der2 = (der1[0] - der1[1]) /
	static_cast<double>(etimeArray[0]);

    // get the effect of gains
    ctrlProp = this->gainProp * error;
    ctrlDer1 = this->gainDer1 * der1[0];
    ctrlDer2 = this->gainDer2 * der2;
    control = -static_cast<int>(ctrlProp + ctrlDer1 + ctrlDer2);

    // adjust motor speed
    if (this->motorsActive) {
	motor.speed(MOTOR_PIN_L, -this->motorSpeed + control);
	motor.speed(MOTOR_PIN_R, this->motorSpeed + control);
    }

    // increase time counters
    for (int i(0); i < 2; ++i)
	this->etimeArray[i] += 1;

    // TODO: check for intersections and update appropriate instance variable
    // TODO: set `turning` and `turnDirection` if making turn
}

void TapeFollow3::makeTurn()
{
    // TODO
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

// TODO
void TapeFollow3::printLCD()
{
}

TapeFollow3::TapeFollow3()
{
    this->init();
}

void TapeFollow3::loop()
{
    if (!this->active)
	return;
    else if (!(this->turning || this->onTape))
	this->seekTape();
    else if (this->turning)
	this->makeTurn();
    else
	this->followTape();
}

void TapeFollow3::start()
{
    this->active = true;
}

void TapeFollow3::stop()
{
    this->init();
    this->active = false;
}

void TapeFollow3::pause()
{
    this->active = false;
}
