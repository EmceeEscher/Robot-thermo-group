///
// OurGraph.hpp
//

#ifndef OUR_GRAPH_HPP
#define OUR_GRAPH_HPP

#include "GraphVertex.hpp"

class OurGraph
{
  private:

    GraphVertex** nodes;
    int numVertices;

  public: 

    OurGraph(); //constructor

    ~OurGraph(); //deconstructor

    /*
     * Returns four-dimensional array of neighbors of given vertex.
     * index 0 = west, 1 = north, 2 = east, 3 = south
     * if an edge exists in that direction, the entry will be the number of
     * the vertex that it connects to, otherwise it will be 0
     */
    int* getNeighbors(int vertexNum);

    void populateGraph();
};

#endif // OUR_GRAPH_HPP
