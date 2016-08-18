///
// PassengerSeek.cpp
//
#include <phys253.h>
#include "pins.hpp"
#include "PassengerSeek.hpp"
#include "TapeFollow.hpp"


//TODO: add code for QSD that's on digital pin

const int *QSD_PINS_SIDES {PASSENGER_SENSORS_SIDES};     // left-back, left-mid, left-front, right-front, right-mid, right-back
const int NUM_PINS_SIDES  {
    sizeof(PASSENGER_SENSORS_SIDES) / sizeof(PASSENGER_SENSORS_SIDES[0])};


const int MAX_REGISTER_PERIOD            {10};  // number of consecutive readings above the threshold required to register a max
const int MAX_NUM_DERIV_REGISTER_PERIOD  {5};  
const int MAX_REGISTER_PERIOD_SHIELDED   {4}; // number of consecutive readings above the threshold required to register a max for QSDs with shielding
const int MAX_NUM_DERIV_REGISTER_PERIOD_SHIELDED  {2}; // number of consecutive (+) derivatives and then (-) derivatives required to achieve a max for QSDs with shielding


const int MAX_REGISTER_THRESHOLD        {350};  // threshold that readings must be above to register
const int DISTANCE_REGISTER_THRESHOLD   {50};
    
static bool active                {false}; // true if active


static bool approachingPassenger  {false};       // true when approaching a passenger
static bool atPassenger           {false};       // true when adjacent to a passenger
static int  passengerSide         {false};       // if atPassenger, specifies the side (-1=left, 1=right)

static bool atMax             [NUM_PINS_SIDES];  // true if the associated pin is at a maximum
static int  pinReadingsPS     [NUM_PINS_SIDES];  // current pin readings
static int  lastpinReadingsPS [NUM_PINS_SIDES];  // pin readings from last loop

static int  numAboveThreshold [NUM_PINS_SIDES];  // number of consecutive reads above threshold for each pin
static int  numPosDeriv       [NUM_PINS_SIDES];  // number of consecutive positive derivatives THE LAST TIME A POSITIVE DERIVATIVE WAS READ
static int  numNegDeriv       [NUM_PINS_SIDES];  // number of consecutive negative or 0 derivatives THE LAST TIME A NEGATIVE DERIVATIVE WAS READ
static bool lastDerivPositive [NUM_PINS_SIDES];  // true if the last derivative was negative


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
        pinReadingsPS[i] = 0;
        lastpinReadingsPS[i] = 0;
        numAboveThreshold[i] = 0;
        numPosDeriv[i] = 0;
        numNegDeriv[i] = 1;
    }
}


bool PassengerSeek::atMaxSideFront()
{
    return atMax[1] || atMax[2];
}


bool PassengerSeek::atMaxSideMiddle()
{
    return atMax[0] || atMax[3];
}


void PassengerSeek::updateMax()
{   
    //for loop uses different values for non-shielded vs. shielded QSDs
    for (int i(0); i < NUM_PINS_SIDES; ++i) {
      if(i == 1 || i == 2){
	      bool aboveThreshold = (numAboveThreshold[i] >= MAX_REGISTER_PERIOD);
	      bool imax = (
		     (!lastDerivPositive[i]) &&
		     (numPosDeriv[i] >= MAX_NUM_DERIV_REGISTER_PERIOD) &&
		     (numNegDeriv[i] >= MAX_NUM_DERIV_REGISTER_PERIOD));
	      // set array
	      atMax[i] = (aboveThreshold && imax);
      }else{
        bool aboveThreshold = (numAboveThreshold[i] >= MAX_REGISTER_PERIOD_SHIELDED);
        bool imax = (
         (!lastDerivPositive[i]) &&
         (numPosDeriv[i] >= MAX_NUM_DERIV_REGISTER_PERIOD_SHIELDED) &&
         (numNegDeriv[i] >= MAX_NUM_DERIV_REGISTER_PERIOD_SHIELDED));
        // set array
        atMax[i] = (aboveThreshold && imax);
      }
    }
}


// TODO
void PassengerSeek::loop()
{

    // Get pin readings
    for (int i(0); i < NUM_PINS_SIDES; ++i) {
        lastpinReadingsPS[i] = pinReadingsPS[i];
        pinReadingsPS[i] = analogRead(QSD_PINS_SIDES[i]);
    }
    
    // Update derivative counts
    for (int i(0); i < NUM_PINS_SIDES; ++i) {
        if ((pinReadingsPS[i] - lastpinReadingsPS[i]) <= 0) {
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
        if (pinReadingsPS[i] <= MAX_REGISTER_THRESHOLD) 
            numAboveThreshold[i] = 0;
        else if (numAboveThreshold[i] < MAX_REGISTER_PERIOD)
            numAboveThreshold[i] += 1;
    
    // Update atMax array
    PassengerSeek::updateMax();
    
    // If at a maximum, signal to stop tape following
    if (PassengerSeek::atMaxSideFront() && !TapeFollow::isTurning())
        approachingPassenger = true;
    else if (PassengerSeek::atMaxSideMiddle() && !TapeFollow::isTurning()) {
        approachingPassenger = false;
        atPassenger = true;
        if (atMax[0])
            passengerSide = -1;
        else
            passengerSide = 1;
    }    

    PassengerSeek::pickDirection();
}

void PassengerSeek::pickDirection(){
    if(!PassengerSeek::atMaxSideMiddle() && !TapeFollow::isTurning()){
      float left = pinReadingsPS[1]-49;
      float right = pinReadingsPS[2];
      if(left > DISTANCE_REGISTER_THRESHOLD && left > right){
        if(right > DISTANCE_REGISTER_THRESHOLD)
          TapeFollow::giveTurnDirection(100, 50, 10);
        else{
          TapeFollow::giveTurnDirection(100, 10, 30);
        }
      }if(right > DISTANCE_REGISTER_THRESHOLD && right > left){
        if(left > DISTANCE_REGISTER_THRESHOLD)
          TapeFollow::giveTurnDirection(50, 100, 10);
        else
          TapeFollow::giveTurnDirection(10, 100, 30);
      }
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

void PassengerSeek::printLCD(){
    LCD.clear();
    LCD.print("FL: ");
    LCD.print(pinReadingsPS[1]);
    LCD.print(" FR: ");
    LCD.print(pinReadingsPS[2]);
    LCD.setCursor(0,1);
    LCD.print("SL: ");
    LCD.print(pinReadingsPS[0]);
    LCD.print(" SR: ");
    LCD.print(pinReadingsPS[3]);
}

