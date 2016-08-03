///
// Graph.hpp
//
// Track location of robot
//

#ifndef GRAPH_HPP
#define GRAPH_HPP

#include "Direction.hpp"

namespace Graph
{
    
    extern Direction graphTurnDirection;

    int getNextCardinalDirection(int currNode);

    int getNextNode(int currNode, int nextDir);

    Direction getTurnDirection(int cardinalDir, int nextCardinalDir);

    void loop();

    void loop(int);

}

#endif
