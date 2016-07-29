///
// AdjacencyListGraph.hpp
//
// Implemenation of the adjacency list representation of a directed graph
//
#ifndef ADJACENCY_LIST_GRAPH_HPP
#define ADJACENCY_LIST_GRAPH_HPP

#include <StandardCplusplus.h>
#include <vector>
#include <map>
#include "Graph.hpp"
#include "GraphVertex.hpp"
#include "GraphNode.hpp"

using std::vector;
using std::map;


class GraphVertexPtrCompare
{

    bool operator() (const GraphVertex *v1, const GraphVertex *v2) const
    {
	return (*v1) < (*v2);
    }
    
};


class AdjacencyListGraph : public Graph
{

private:

    vector<GraphNode*> nodes;
    vector<GraphVertex*> vertices;
    map<GraphVertex*, GraphNode*, GraphVertexPtrCompare> nodeMap;

public:

    AdjacencyListGraph();

    ~AdjacencyListGraph();

    void addVertex(GraphVertex &v);

    void addEdge(GraphVertex &v1, GraphVertex &v2);

    bool edgeExists(GraphVertex &v1, GraphVertex &v2);

    vector<GraphVertex*>& getDownstreamNeighbors(GraphVertex &v);

    vector<GraphVertex*>& getUpstreamNeighbors(GraphVertex &v);

    vector<GraphVertex*>& getVertices();
    
};


#endif // ADJACENCY_LIST_GRAPH_HPP
