///
// UndirectedGraph.hpp
//
// Abstract class for an undirected (symmetric) graph
//
#ifndef UNDIRECTED_GRAPH_HPP
#define UNDIRECTED_GRAPH_HPP

#include <vector>
#include "Graph.hpp"
#include "GraphVertex.hpp"

class UndirectedGraph : public Graph
{

private:

    /*
     * Functionality hidden, since this is the same as getNeighbors()
     */
    std::vector<GraphVertex*> getDownstreamNeighbors(GraphVertex &v)
    {
	return this->getNeighbors(v);
    }

    /*
     * Functionality hidden, since this is the same as getNeighbors()
     */
    std::vector<GraphVertex*> getUpstreamNeighbors(GraphVertex &v)
    {
	return this->getNeighbors(v);
    }
    
public:

    virtual ~UndirectedGraph() {}  // deconstructor

    /*
     * Returns a vector of all of the neighbors adjacent to the vertex v.
     * Assumes v is a vertex in the graph.
     */
    virtual std::vector<GraphVertex*> getNeighbors(GraphVertex &v) = 0;

};

#endif  // UNDIRECTED_GRAPH_HPP
