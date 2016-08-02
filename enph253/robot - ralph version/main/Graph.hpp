///
// Graph.hpp
//
// Track location of robot
//

#ifndef GRAPH_HPP
#define GRAPH_HPP

#include "Direction.hpp"

int getNextCardinalDirection(int);

int getNextNode(int, int);

Direction getTurnDirection(int, int);

void graphLoop();

void graphLoop(int);

#endif
