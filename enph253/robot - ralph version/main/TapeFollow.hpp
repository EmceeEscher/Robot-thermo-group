#ifndef TAPE_FOLLOW_HPP
#define TAPE_FOLLOW_HPP

#include "Direction.hpp"

  void tapeFollowInit();

  double seekTape();

  void intersectionSeen();

  void intersectionDetection();

  double followTape();

  bool fnAllLastReadings(int period, int fn);

  bool fnAnyLastReadings(int period, int fn);

  bool offTape(bool reading[]);

  bool mainsOffTape(bool reading[]);

  bool intsOffTape(bool reading[]);

  bool intLOnTape(bool reading[]);

  bool intROnTape(bool reading[]);

  double makeTurn();

  Direction chooseTurn(bool left, bool right, bool straight);

  void printLCD();

  void tapeFollowLoop();

  void start();

  void stop();

  void pause();

  bool isActive();

  void test();

  void giveTurnDirection(float left, float right, float straight);

  void turnAround();

#endif
