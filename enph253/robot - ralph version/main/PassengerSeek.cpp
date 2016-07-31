///
// PassengerSeek.cpp
//
#include <phys253.h>
#include "pins.hpp"
#include "PassengerSeek.hpp"


//TODO: add code for QSD that's on digital pin

// const int *QSD_PINS_SIDES {pins::PASSENGER_SENSORS_SIDES};     // left-back, left-mid, left-front, right-front, right-mid, right-back
// const int NUM_PINS_SIDES  {pins_sizes::PASSENGER_SENSORS_SIDES};
const int *QSD_PINS_SIDES {PASSENGER_SENSORS_SIDES};     // left-back, left-mid, left-front, right-front, right-mid, right-back
const int NUM_PINS_SIDES  {sizeof(PASSENGER_SENSORS_SIDES) /
	sizeof(PASSENGER_SENSORS_SIDES[0])};

const int MAX_REGISTER_PERIOD            {20};  // number of consecutive readings above the threshold required to register a max
const int MAX_NUM_DERIV_REGISTER_PERIOD  {10};  // number of consecutive (+) derivatives and then (-) derivatives required to achieve a max
const int MAX_REGISTER_THRESHOLD        {600};  // threshold that readings must be above to register
    
static bool active                {false}; // true if active

static bool approachingPassenger  {false}; // true when approaching a passenger
static bool atPassenger           {false}; // true when adjacent to a passenger
static int  passengerSide         {false}; // if atPassenger, specifies the side (-1=left, 1=right)

static bool atMax           [NUM_PINS_SIDES];    // true if the associated pin is at a maximum
static int  pinReadings     [NUM_PINS_SIDES];    // current pin readings
static int  lastPinReadings [NUM_PINS_SIDES];    // pin readings from last loop

static int  numAboveThreshold [NUM_PINS_SIDES];  // number of consecutive reads above threshold for each pin
static int  numPosDeriv       [NUM_PINS_SIDES];  // number of consecutive positive derivatives THE LAST TIME A POSITIVE DERIVATIVE WAS READ
static int  numNegDeriv       [NUM_PINS_SIDES];  // number of consecutive negative or 0 derivatives THE LAST TIME A NEGATIVE DERIVATIVE WAS READ
static bool lastDerivPositive [NUM_PINS_SIDES];  // true if the last derivative was negative


namespace PassengerSeek
{
    static void init();
    static bool atMaxSideFront();
    static bool atMaxSideMiddle();
    static void updateMax();
}

    
void PassengerSeek::init()
{
    active = false;
    approachingPassenger = false;
    atPassenger = false;
    passengerSide = 0;  // undefined
    
    for (int i(0); i < NUM_PINS_SIDES; ++i) {
	atMax[i] = false;
	lastDerivPositive[i] = false;
    }
    
    for (int i(0); i < NUM_PINS_SIDES; ++i) {
	pinReadings[i] = 0;
	lastPinReadings[i] = 0;
	numAboveThreshold[i] = 0;
	numPosDeriv[i] = 0;
	numNegDeriv[i] = 1;
    }
}
    
    
bool PassengerSeek::atMaxSideFront()
{
    return atMax[2] || atMax[3];
}
    
    
bool PassengerSeek::atMaxSideMiddle()
{
    return atMax[1] || atMax[4];
}
    
    
    // TODO
void PassengerSeek::updateMax()
{
    for (int i(0); i < NUM_PINS_SIDES; ++i) {
	bool aboveThreshold = (numAboveThreshold[i] >= MAX_REGISTER_PERIOD);
	bool imax = (
		     (!lastDerivPositive[i]) &&
		     (numPosDeriv[i] >= MAX_NUM_DERIV_REGISTER_PERIOD) &&
		     (numNegDeriv[i] >= MAX_NUM_DERIV_REGISTER_PERIOD));
	// set array
	atMax[i] = static_cast<int>(aboveThreshold && imax);
    }
}


// TODO
void PassengerSeek::loop()
{
    static bool initialized(false);

    // Initialize, if not yet done
    if (!initialized) {
	PassengerSeek::init();
	initialized = true;
    }

    // Get pin readings
    for (int i(0); i < NUM_PINS_SIDES; ++i) {
	lastPinReadings[i] = pinReadings[i];
	pinReadings[i] = analogRead(QSD_PINS_SIDES[i]);
    }
    
    // Update derivative counts
    for (int i(0); i < NUM_PINS_SIDES; ++i) {
	if ((pinReadings[i] - lastPinReadings[i]) <= 0) {
	    if (lastDerivPositive[i])
		numNegDeriv[i] = 1;
	    else if (numNegDeriv[i] < MAX_NUM_DERIV_REGISTER_PERIOD)
		numNegDeriv[i] += 1;
	    lastDerivPositive[i] = false;
	} else {
	    if (!lastDerivPositive[i])
		numPosDeriv[i] = 1;
	    else if (numPosDeriv[i] < MAX_NUM_DERIV_REGISTER_PERIOD)
		numPosDeriv[i] += 1;
	    lastDerivPositive[i] = true;
	}
    }

    // Update counters
    for (int i(0); i < NUM_PINS_SIDES; ++i)
	if (pinReadings[i] <= MAX_REGISTER_THRESHOLD) {
	    numAboveThreshold[i] = 0;
	} else if (numAboveThreshold[i] < MAX_REGISTER_PERIOD)
	    numAboveThreshold[i] += 1;

    // Update atMax array
    PassengerSeek::updateMax();

    // If at a maximum, signal to stop tape following
    if (PassengerSeek::atMaxSideFront())
	approachingPassenger = true;
    else if (PassengerSeek::atMaxSideMiddle()) {
	approachingPassenger = false;
	atPassenger = true;
    }
}


void PassengerSeek::start()
{
    active = true;
}


void PassengerSeek::stop()
{
    PassengerSeek::init();
    PassengerSeek::pause();
}


void PassengerSeek::pause()
{
    active = false;
}


void PassengerSeek::test()
{
    active = true;
}


bool PassengerSeek::isActive()
{
    return active;
}


bool PassengerSeek::isApproachingPassenger()
{
    return approachingPassenger;
}


bool PassengerSeek::isAtPassenger()
{
    return atPassenger;
}


int PassengerSeek::getPassengerSide()
{
    return passengerSide;
}
