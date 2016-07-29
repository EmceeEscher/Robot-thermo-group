///
// graphalgs.hpp
//
// Algorithms for searching graphs
//
#ifndef GRAPHALGS_HPP
#define GRAPHALGS_HPP

#include "Graph.hpp"
#include "GraphVertx.hpp"


namespace graphs
{

    /*
     * Returns the shortest distance between vertex v1 and v2 in the given
     * graph.
     *
     * param graph: a graph
     * param v1: the starting vertex
     * param v2: the final vertex
     * returns: smallest number of edges for a downstream path from v1 to v2,
     * 0 if they are the same vertex, or -1 if no path exists
     */
    int shortestDistance(Graph &graph, GraphVertex &v1, GraphVertex &v2);

    
    
}


#endif // GRAPHALGS_HPP
