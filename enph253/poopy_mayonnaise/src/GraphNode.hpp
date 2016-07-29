///
// GraphNode.hpp
//
// Node for an AdjacencyListGraph
//
#ifndef GRAPH_NODE_HPP
#define GRAPH_NODE_HPP

// #include <StandardCplusplus.h>
#include <vector>
#include "GraphVertex.hpp"

using std::vector;

class GraphNode
{

private:

    GraphVertex& self;  // the vertex that this node wraps
    vector<GraphVertex*> upstreamNeighbors;    // upstream vertices
    vector<GraphVertex*> downstreamNeighbors;  // downstream vertices

public:

    /*
     * Initialize the node with the given vertex
     */
    GraphNode(GraphVertex &v);

    /*
     * Add the given vertex as an upstream neighbor
     */
    void addUpstreamNeighbor(GraphVertex &v);

    /*
     * Add the given vertex as a downstream neighbor
     */
    void addDownstreamNeighbor(GraphVertex &v);

    /*
     * Returns a reference to the vector of upstream neighbors
     */
    vector<GraphVertex*>& getUpstreamNeighbors();

    /*
     * Returns a reference to the vector of downstream neighbors
     */
    vector<GraphVertex*>& getDownstreamNeighbors();

    /*
     * Returns a refernce to the vertex that this node wraps
     */
    GraphVertex& getNodeVertex();

};

#endif // GRAPH_NODE_HPP
