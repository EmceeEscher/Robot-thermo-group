#ifndef FIND_BEACON_HPP
#define FIND_BEACON_HPP

#include "pins.hpp"
#include "Direction.hpp"

namespace FindBeacon{
  /*
   * Initialiazes beacon readings to be 0
   */
  void init();

  /*
   * Picks a direction to turn next depending on the location of the beacon relative to the robot.
   * POST-COMPETITION UPDATE: because of the ridiculous swinging of the arm on which the sensors
   * were mounted, we ended up not being able to turn towards the beacon, and instead just
   * turned randomly at each intersection
   */
  void pickDirection();

  /*
   * Takes the reading from each beacon sensor, and stores that value in an array of previous readings
   */
  void loop();

  /*
   * Reads the IR signal coming into the left beacon sensor, adds that value to the array of previous
   * readings, and removes the oldest value. Also updates the sum of all the stored readings
   */
  void updateLeftBeaconArray();

  /*
   * Reads the IR signal coming into the right beacon sensor, adds that value to the array of previous
   * readings, and removes the oldest value. Also updates the sum of all the stored readings
   */
  void updateRightBeaconArray();

  /*
   * Returns which direction the robot thinks the beacon is in, depending on the values read by the IR
   * sensors relative to a set threshold
   */
  Direction getBeaconDirection();

  /*
   * Returns the value currently read by the left beacon detector
   */
  int getLeftBeaconReading();

  /*
   * Returns the value currently read by the right beacon detector
   */
  int getRightBeaconReading();
  
  /*
   * Returns the average of the last numBeaconReadings values read by the left beacon sensor
   */
  float getLeftBeaconAverage();

  /*
   * Returns the average of the last numBeaconReadings values read by the right beacon sensor
   */
  float getRightBeaconAverage();

  /*
   * returns true if the robot has gotten close enough to the beacon for either of the beacon sensors'
   * readings to surpass the arrival threshold
   */
  bool hasArrived();

  /*
   * prints the readings and averages of both beacon sensors
   */
  void printLCD();
}

#endif // FIND_BEACON_HPP
