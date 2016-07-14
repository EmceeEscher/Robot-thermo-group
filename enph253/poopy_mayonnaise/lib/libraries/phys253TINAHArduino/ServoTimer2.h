/*
  ServoTimer2.h - Interrupt driven Servo library for Arduino using

  2015Jan2 - edits by Jon Nakane to operate under Arduino IDE with TINAH board (changed Timer2 registers to match with
  ATMega128 register names.  Also, changed min and max pulse width values.
  
  2016-07-13 - Dilyn Fullerton
  Some serious clean up. All declarations here and definitions in the 
  associated cpp file.

  This library uses Timer2 to drive up to 8 servos using interrupts so no refresh activity is required from within the sketch.
  The usage and method naming is similar to the Arduino software servo library http://www.arduino.cc/playground/ComponentLib/Servo
  except that pulse widths are in microseconds.


  A servo is activated by creating an instance of the Servo class passing the desired pin to the attach() method.
  The servo is pulsed in the background to the value most recently written using the write() method

  Note that analogWrite of PWM on pins 3 and 11 is disabled when the first servo is attached

  The methods are:

   ServoTimer2 - Class for manipulating servo motors connected to Arduino pins.

   attach(pin )  - Attaches a servo motor to an i/o pin.
   attach(pin, min, max  ) - Attaches to a pin setting min and max values in microseconds

   write()     - Sets the servo pulse width in microseconds.

   read()       - Gets the last written servo pulse width in microseconds.

   attached()  - Returns true if there is a servo attached.

   detach()    - Stops an attached servos from pulsing its i/o pin.


The library takes about 824 bytes of program memory and 32+(1*servos) bytes of SRAM.
The pulse width timing is accurate to within 1%

*/

// ensure this library description is only included once
#ifndef ServoTimer2_h
#define ServoTimer2_h

#include <inttypes.h>

#define MIN_PULSE_WIDTH       400          // the shortest pulse sent to a servo
#define MAX_PULSE_WIDTH      1500          // the longest pulse sent to a servo
#define DEFAULT_PULSE_WIDTH   750          // default pulse width when servo is attached
#define FRAME_SYNC_PERIOD   10000 //20000  // total frame duration in microseconds
#define NBR_CHANNELS            8          // the maximum number of channels, don't change this

typedef uint8_t boolean;
typedef uint8_t byte;

typedef struct  {
    uint8_t nbr      :6 ;  // a pin number from 0 to 63
    uint8_t isActive :1 ;  // false if this channel not enabled, pin only pulsed if true
} ServoPin_t   ;


typedef struct {
    ServoPin_t Pin;
    byte counter;
    byte remainder;
}  servo_t;


class ServoTimer2
{

public:
    // constructor:
    ServoTimer2();

    /* 
     * attach the given pin to the next free channel, sets pinMode, returns 
     * channel number or 0 if failure
     */
    uint8_t attach(int);           

    void detach();

    void write(int);               

    /* 
     * returns current pulse width in microseconds for this servo
     */
    int read();                    

    /* 
     * return true if this servo is attached
     */
    boolean attached();           

private:
    uint8_t chanIndex;  // index into the channel data for this servo

    /* 
     * store the pulse width in microseconds (between MIN_PULSE_WIDTH and 
     * MAX_PULSE_WIDTH)for this channel
     */
    void writePulseWidth(int);  

};


// the following ServoArrayT2 class is not implemented in the first version of this library
class ServoArrayT2
{

public:
    // constructor:
    ServoArrayT2();

    /*
     * Attach the given pin to the next free channel, sets pinMode, 
     * returns channel number or 0 if failure.
     * Channels are assigned consecutively starting from 1.
     * The attached servo is pulsed with the current pulse width value, 
     * (see the write method)
     */
    uint8_t attach(int);      

    /*
     * detach the servo on the given channel
     */
    void detach(int);         

    /* 
     * store the pulse width in microseconds (between MIN_PULSE_WIDTH and 
     * MAX_PULSE_WIDTH)for the given channel
     */
    void write(int,int);      

    /*
     * returns current pulse width in microseconds for the given channel
     */
    int read(int);            

    /* 
     * return true if the servo on the given channel is attached
     */
    boolean attached(int);   

private:
    void writePulseWidth(int,int) ;
    uint8_t chanIndex;     // index into the channel data for this servo

};

#endif
