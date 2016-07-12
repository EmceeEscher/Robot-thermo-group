///
// TapeFollow.cpp
//
#include "TapeFollow.hpp"

//#pragma clang diagnostic push
//#pragma ide diagnostic ignored "CannotResolve"

// constants
// sensors
const int TAPE_SENSOR_PINS_FRONT[] {0, 1, 2, 3};
const int TAPE_SENSOR_PINS_BACK[]  {4, 5, 6, 7};
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
const int PRINT_PERIOD(270);

// Class constructor
TapeFollow::TapeFollow()
    : smallError(SMALL_ERROR),
      largeError(LARGE_ERROR),
      motorSpeed(MOTOR_SPEED),
      resetPeriod(RESET_PERIOD),
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
      count(0),
      motorsActive(false)
{
    portMode(0, INPUT);
    // set instance arrays
    this->prevIntersections[0] = false;
    this->prevIntersections[1] = false;
    for (int i = 0; i < 4; ++i) {
        this->activePins[i] = TAPE_SENSOR_PINS_FRONT[i];
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
    static int control;
    static float error;
    static bool intersectionDetected[2];
    static bool pinReadings[4];
    static bool onTape(false);
    const static bool &mainL = pinReadings[1];          // main left reading
    const static bool &mainR = pinReadings[2];          // main right reading
    const static bool &intersectionL = pinReadings[0];  // left intersection reading
    const static bool &intersectionR = pinReadings[3];  // right intersection reading

    // get proportional and dervative gains from knobs
    propGain = knob(this->propGainKnob) / 50;
    dervGain = knob(this->dervGainKnob) / 50;

    // get readings from tape sensors
    for (int i = 0; i < 4; ++i)
        pinReadings[i] = static_cast<bool>(digitalRead(this->activePins[i]));
    onTape = (mainL || mainR);

    // determine error
    if (mainL && mainR)  // both mains over tape
        error = 0;
    else if (mainL)      // left main over tape
        error = -this->smallError;
    else if (mainR)      // right main over tape
        error = this->smallError;
    else if (this->lastError > 0)
        error = this->largeError;
    else if (this->lastError < 0)
        error = -this->largeError;
    else
	error = 0;

    if (error != this->lastError) {
        this->recentError = this->lastError;
        this->prevTime = this->timeStep;
        this->timeStep = 1;
    }

    // do intersection stuff if on tape
    if (mainL && mainR) {
	// record intersection if seen
	if (intersectionL && (this->lastError <= 0))
	    this->prevIntersections[0] = true;
	else if (intersectionR && (this->lastError >= 0))
	    this->prevIntersections[1] = true;
	else if ((!(intersectionL)) && (this->prevIntersections[0]))
	    intersectionDetected[0] = true;
	else if ((!(intersectionR)) && (this->prevIntersections[1]))
	    intersectionDetected[1] = true;
	else {
	    intersectionDetected[0] = false;
	    intersectionDetected[1] = false;
	}

	 // decide which direction to go in
	 // turnDirection is either 0 (left), 1 (right), or 2 (straight)
	 if (intersectionDetected[0] && intersectionDetected[1])
	     // this->turnDirection = static_cast<int>(random(3));
	     this->turnDirection = 1;
	 else if (intersectionDetected[0])
	     // this->turnDirection = 2 * static_cast<int>(random(2));
	     this->turnDirection = 0;
	 else if (intersectionDetected[1])
	     // this->turnDirection = 1 + static_cast<int>(random(2));
	     this->turnDirection = 1;
	 else
	     this->turnDirection = 2;
	
	 // make turn by changing error
	 if (this->turnDirection == 0)
	     error = -this->largeError;
	 else if (this->turnDirection == 1)
	     error = this->largeError;
    }


    // get net effect of proportional and derivative gains
    prop = (propGain * error);
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
    if (this->motorsActive) {
	motor.speed(this->motorPinL, -this->motorSpeed + control);
	motor.speed(this->motorPinR, this->motorSpeed + control);
    }
    this->lastError = error;

    // print crap
    if (this->count % PRINT_PERIOD == 0) {
	LCD.clear();
        if (control < 0) 
	    LCD.print("<-- ");
	else if (control > 0) 
	    LCD.print("--> ");
	else 
	    LCD.print(" ^  ");

	LCD.print(intersectionL);
	LCD.print(" ");
	LCD.print(mainL);
	LCD.print(" ");
	LCD.print(mainR);
	LCD.print(" ");
	LCD.print(intersectionR);
	LCD.setCursor(0,1);
	LCD.print(propGain);
	LCD.print(" ");
	LCD.print(dervGain);
	LCD.print(" ");
	LCD.print(control);
    }
}

void TapeFollow::start() {
    this->motorsActive = true;
}

void TapeFollow::stop() {
    this->motorsActive = false;
    motor.speed(this->motorPinL, 0);
    motor.speed(this->motorPinR, 0);
}


//#pragma clang diagnostic pop
