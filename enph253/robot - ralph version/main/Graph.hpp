///
// Graph.hpp
//
// Track location of robot
//

#ifndef GRAPH_HPP
#define GRAPH_HPP

#include "Direction.hpp"

extern Direction graphTurnDirection;

int getNextCardinalDirection();

int getNextNode();

Direction getTurnDirection();

void graphLoop();

void graphLoop(int);

#endif
