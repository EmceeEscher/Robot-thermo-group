#ifndef TAPE_FOLLOW_HPP
#define TAPE_FOLLOW_HPP

#include "Direction.hpp"

namespace TapeFollow{

  /*
   * Initializes all variables to their starting values
   */
  void init();

  /*
   * Returns the error value used to determine motor control
   * when the robot is off of tape unexpectedly
   */
  double seekTape();

  /*
   * Sets variables according to whether or not the robot has detected an
   * intersection with either of its intersection detectors
   */
  void intersectionSeen();

  /*
   * Controls the robot's behavior at intersections, telling it it to turn
   * or go straight once it has verified that it has seen an intersection, and 
   * depending on the options available to it
   */
  void intersectionDetection();

  /*
   * Returns the error value used to determine motor control when the
   * robot is tape following
   */
  double followTape();

  /*
   * Returns true if the given function was true for the all points in the given period
   * @param period: the time interval over which you're checking
   * @param fn: the function you are checking (use constants defined in CPP)
   *    valid constants: FN_MAINS_OFF_TAPE, FN_INTS_OFF_TAPE, FN_OFF_TAPE
   */
  bool fnAllLastReadings(int period, int fn);

  /*
   * Returns true if the given function was true for any point in the given period
   * @param period: the time interval over which you're checking
   * @param fn: the function you are checking (use constants defined in CPP)
   *    valid constants: FN_MAINS_OFF_TAPE, FN_INTS_OFF_TAPE, FN_OFF_TAPE
   */
  bool fnAnyLastReadings(int period, int fn);

  /*
   * returns true if all of the tape-detecting sensors are off-tape during the reading given
   * @param reading: the concurrent reading of all four sensors that you're checking
   */
  bool offTape(bool reading[]);

  /*
   * returns true if both of the middle tape-detecting sensors are off-tape during the reading given
   * @param reading: the concurrent reading of all four sensors that you're checking
   */
  bool mainsOffTape(bool reading[]);

  /*
   * returns true if both of the intersection tape-detecting sensors are off-tape during the reading given
   * @param reading: the concurrent reading of all four sensors that you're checking
   */
  bool intsOffTape(bool reading[]);

  /*
   * returns true if the left intersection tape-detecting sensor is off-tape during the reading given
   * @param reading: the concurrent reading of all four sensors that you're checking
   */
  bool intLOnTape(bool reading[]);

  /*
   * returns true if the right intersection tape-detecting sensor is off-tape during the reading given
   * @param reading: the concurrent reading of all four sensors that you're checking
   */
  bool intROnTape(bool reading[]);

  /*
   * returns the error value needed for motor control while the robot is making a turn
   */
  double makeTurn();

  /*
   * Randomly picks a direction to turn at the next intersection, weighted by the values stored as 
   * leftWeight, rightWeight, and straightWeight. Won't return a direction that is not a valid path.
   * @params: true if that direction is a valid path (i.e. has tape going off in that direction), false otherwise
   */
  Direction chooseTurn(bool left, bool right, bool straight);

  /*
   * Prints the readings of the tape-following sensors on the screen, as well as what direction the 
   * robot is trying to turn
   */
  void printLCD();

  /*
   * The main control loop for the tape following process
   */
  void loop();

  /*
   * Tells the robot to begin tape-following
   */
  void start();

  /*
   * Tells the robot to stop tape-following
   */
  void stop();

  /*
   * Tells the robot to pause tape-following. (motors will stop moving,
   * but sensors will continue updating)
   */
  void pause();

  /*
   * returns true if tape-following has been started
   */
  bool isActive();

  /*
   * Sets the weights that the robot uses to decide which direction to turn. 
   * 
   * For deterministic turning: give first option weight of 100, second option weight of 0.1, third option weight of 0
   * (known bug: will produce wrong result 0.1% of the time, but not worth fixing currently)
   */
  void giveTurnDirection(float left, float right, float straight);

  /*
   * Causes the robot to turn around in place and go in the opposite direction
   */
  void turnAround();

  /*
   * Returns true if the robot is currently in the process of turning
   */
  bool isTurning();

  /*
   * Returns the number of loops that have passed since the robot started turning around
   * (behavior undefined if the robot is not currently turning around)
   */
  float getTimeTurningAround();

}

#endif
