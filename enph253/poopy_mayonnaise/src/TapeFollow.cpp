///
// TapeFollow.cpp
//
#include "TapeFollow.hpp"

//#pragma clang diagnostic push
#pragma ide diagnostic ignored "CannotResolve"

// constants
// sensors
//const int TAPE_SENSOR_FOL(0);  // front, outside, left
//const int TAPE_SENSOR_FIL(1);  // front, inside,  left
//const int TAPE_SENSOR_FIR(2);  // front, inside,  right
//const int TAPE_SENSOR_FOR(3);  // front, outside, right
//const int TAPE_SENSOR_BOL(4);  // back,  outside, left
//const int TAPE_SENSOR_BIL(5);  // back,  inside,  left
//const int TAPE_SENSOR_BIR(6);  // back,  inside,  right
//const int TAPE_SENSOR_BOR(7);  // back,  outside, right
const int MOTOR_PIN_L(0);      // left motor pin
const int MOTOR_PIN_R(3);      // right motor pin
// knobs
const int PROP_GAIN_KNOB(6);
const int DERV_GAIN_KNOB(7);
// other
const float SMALL_ERROR(1.0);
const float LARGE_ERROR(10.0);
const int MOTOR_SPEED(100);
const int RESET_PERIOD(300);
const int THRESHOLD(45);


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
      prevTime(0),
      timeStep(0),
      error(0),
      lastError(0),
      recentError(0),
      tinah(t),
      count(0)
{
    portMode(0, INPUT);
    // set instance arrays
    this->intersections[0] = 0;
    this->intersections[1] = 0;
    for (int i = 0; i < 4; ++i) {
        this->activePins[i] = i;
	    pinMode(i, INPUT);
    }

}

// Main loop function
void TapeFollow::loop() {
    // declare static variables (runs only once)
    static double propGain;  // proportional gain
    static double dervGain;  // derivative gain
    static double prop;      // proportional contribution to control
    static double derv;      // derivative contribution to control
    static double mainL;     // main left reading
    static double mainR;     // main right reading
    static double intersectionL;   // left intersection reading
    static double intersectionR;   // right intersection reading
    static int control;      
    static float error;
    
    // get proportional and dervative gains from knobs
    propGain = knob(this->propGainKnob) / 50;
    dervGain = knob(this->dervGainKnob) / 50;

    // get readings from tape sensors
    for (int i = 0; i < 4; ++i)
	this->pinReadings[i] = analogRead(this->activePins[i]);
    mainL = this->pinReadings[1];
    mainR = this->pinReadings[2];
    intersectionL = this->pinReadings[0];
    intersectionR = this->pinReadings[3];

    // determine error
    if ((mainL > this->threshold) && (mainR > this->threshold))
        error = 0;
    else if (mainL > this->threshold)
        error = -this->smallError;
    else if (mainR > this->threshold)
        error = this->smallError;
    else if (this->lastError > 0)
        error = this->largeError;
    else
        error = -this->largeError;

    if (error != this->lastError) {
        this->recentError = this->lastError;
        this->prevTime = this->timeStep;
        this->timeStep = 1;
    }

    // record intersection if seen
    if ((intersectionL > this->threshold) && (this->lastError <= 0))
        this->intersections[0] = 1;
    else if ((intersectionR > this->threshold) && (this->lastError >= 0))
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
        error = -this->largeError;
    else if (this->turnDirection == 1)
        error = this->largeError;
    
    // get net effect of proportional and derivative gains
    prop = (propGain * this->error);
    derv = (static_cast<float>(dervGain) *
	    static_cast<float>(error - this->recentError)
	    / static_cast<float>(this->prevTime - this->timeStep));
    control = -static_cast<int>(prop + derv);

    // increase counters
    if (this->count == this->resetPeriod) 
        this->count = 0;
    this->count = this->count + 1;
    this->timeStep = this->timeStep + 1;

    // adjust motor speed
    this->tinah.motor.speed(this->motorPinL, -this->motorSpeed + control);
    this->tinah.motor.speed(this->motorPinR, this->motorSpeed + control);
    this->lastError = error;

    // print crap
    if (this->count % 270 == 0) {
        if (control < 0.0) {
	        this->tinah.LCD.clear();
	        this->tinah.LCD.print("<-- ");
	        this->tinah.LCD.print(mainL);
	        this->tinah.LCD.print(" ");
	        this->tinah.LCD.print(mainR);
	    } else if (control > 0.0) {
	        this->tinah.LCD.clear();
	        this->tinah.LCD.print("--> ");
	        this->tinah.LCD.print(mainL);
	        this->tinah.LCD.print(" ");
	        this->tinah.LCD.print(mainR);
	    } else {
	        this->tinah.LCD.clear();
	        this->tinah.LCD.print("-^- ");
	        this->tinah.LCD.print(mainL);
	        this->tinah.LCD.print(" ");
	        this->tinah.LCD.print(mainR);
	    }
    }
}

//#pragma clang diagnostic pop