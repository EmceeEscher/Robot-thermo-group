///
// TapeFollow.cpp
//
#include "TapeFollow.h"


// constants
// sensors
const int TAPE_SENSOR_FOL = 0;  // front, outside, left
const int TAPE_SENSOR_FIL = 1;  // front, inside,  left
const int TAPE_SENSOR_FIR = 2;  // front, inside,  right
const int TAPE_SENSOR_FOR = 3;  // front, outside, right
const int TAPE_SENSOR_BOL = 4;  // back,  outside, left
const int TAPE_SENSOR_BIL = 5;  // back,  inside,  left
const int TAPE_SENSOR_BIR = 6;  // back,  inside,  right
const int TAPE_SENSOR_BOR = 7;  // back,  outside, right
const int MOTOR_PIN_L = 0;      // left motor pin
const int MOTOR_PIN_R = 3;      // right motor pin
// knobs
const int PROP_GAIN_KNOB = 6;
const int DERV_GAIN_KNOB = 7;
// other
const float SMALL_ERROR = 1.0;
const float LARGE_ERROR = 10.0;
const int MOTOR_SPEED = 200;
const int RESET_PERIOD = 30;
const int THRESHOLD = 45;


// Class constructor
TapeFollow::TapeFollow(Tinah &t)
    : smallError(SMALL_ERROR),
      largeError(LARGE_ERROR),
      motorSpeed(MOTOR_SPEED),
      resetPeriod(RESET_PERIOD),
      threshold(THRESHOLD),
      propGainKnob(PROP_GAIN_KNOB),
      dervGainKnob(DERV_GAIN_KNOB),
      motorPinL(MOTOR_PIN_L),
      motorPinR(MOTOR_PIN_R),
      turnDirection(2),
      direction(0),
      count(0),
      prevTime(0),
      timeStep(0),
      error(0),
      lastError(0),
      recentError(0),
      tinah(t)
{
    // set instance arrays
    this->intersections[0] = 0;
    this->intersections[1] = 0;
    for (int i = 0; i < 4; ++i)
	this->activePins[i] = i;
}

// Main loop function
void TapeFollow::loop() {
    // get proportional and dervative gains from knobs
    this->propGain = knob(this->propGainKnob) / 50;
    this->dervGain = knob(this->dervGainKnob) / 50;

    // get readings from tape sensors
    for (int i = 0; i < 4; ++i)
	this->pinReadings[i] = analogRead(this->activePins[i]);

    double mainL = this->pinReadings[1];
    double mainR = this->pinReadings[2];
    double intersectionL = this->pinReadings[0];
    double intersectionR = this->pinReadings[3];

    // determine error
    if ((mainL > this->threshold) && (mainR > this->threshold))
	this->error = 0;
    else if (mainL > this->threshold)
	this->error = -this->smallError;
    else if (mainR > this->threshold)
	this->error = this->smallError;
    else if (lastError > 0)
	this->error = this->largeError;
    else
	this->error = -this->largeError;

    if (this->error != this->lastError) {
	this->recentError = this->lastError;
	this->prevTime = this->timeStep;
	this->timeStep = 1;
    }

    // record intersection if seen
    if ((intersectionL > this->threshold) && (lastError <= 0))
	this->intersections[0] = 1;
    else if ((intersectionR > this->threshold) && (lastError >= 0))
	this->intersections[1] = 1;
    else {
	this->intersections[0] = 0;
	this->intersections[1] = 0;
    }

    // decide which direction to go in
    // turnDirection is either 0 (left), 1 (right), or 2 (straight)
    if (this->intersections[0] && this->intersections[1])
	this->turnDirection = static_cast<int>(random(3));
    else if (this->intersections[0])
	this->turnDirection = 2 * static_cast<int>(random(2));
    else if (this->intersections[1])
	this->turnDirection = 1 + static_cast<int>(random(2));
    else
	this->turnDirection = 2;

    // make turn by changing error
    if (this->turnDirection == 0)
	this->error = -this->largeError;
    else if (this->turnDirection == 1)
	this->error = this->largeError;
    
    // get net effect of proportional and derivative gains
    this->prop = static_cast<int>(propGain * this->error);
    this->derv = static_cast<int>(static_cast<float>(dervGain) *
				 static_cast<float>(this->error -
						    this->recentError)
				 / static_cast<float>(this->prevTime -
						      this->timeStep));
    this->control = -(this->prop + this->derv);

    // increase counters
    if (this->count == this->resetPeriod) 
	this->count = 0;
    this->count = this->count + 1;
    this->timeStep = this->timeStep + 1;

    // adjust motor speed
    this->tinah.motor.speed(this->motorPinL, -this->motorSpeed + control);
    this->tinah.motor.speed(this->motorPinR, this->motorSpeed + control);
    this->lastError = this->error;
}
