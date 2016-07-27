///
// Graph.hpp
//
// Interface for a graph.
// A port of the Java interface definition from the CPEN 221 staff for
// Machine Problem 3.
//
#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <vector>
#include "GraphVertex.hpp"

class Graph
{

public:

    virtual ~Graph() {}  // deconstructor

    /*
     * Adds the given vertex to the graph.
     * Assumes the vertex is not already in the graph.
     */
    virtual void addVertex(GraphVertex &v) = 0;

    /*
     * Adds the given edge to the graph between v1 and v2.
     * Assumes v1 and v2 are already vertices in the graph.
     */
    virtual void addEdge(GraphVertex &v1, GraphVertex &v2) = 0;

    /*
     * Returns true if there is an edge between v1 and v2.
     * Assumes v1 and v2 are vertices in the graph.
     */
    virtual bool edgeExists(GraphVertex &v1, GraphVertex &v2) = 0;

    /*
     * Returns a vector of all of the downstream vertices adjacent to the 
     * given vertex v.
     * Assumes v is a vertex in the graph.
     */
    virtual std::vector<GraphVertex*> getDownstreamNeighbors(
            GraphVertex &v) = 0;

    /*
     * Returns a vector of all of the upstream vertices adjacent to the given
     * vertex v.
     * Assumes v is a vertex in the graph.
     */
    virtual std::vector<GraphVertex*> getUpstreamNeighbors(
            GraphVertex &v) = 0;

    /*
     * Returns a vector of all of the vertices in the graph.
     */
    virtual std::vector<GraphVertex*> getVertices() = 0;

};

#endif  // GRAPH_HPP
