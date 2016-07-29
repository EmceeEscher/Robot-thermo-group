///
// AdjacencyListGraph.cpp
//

// #include <StandardCplusplus.h>
#include <vector>
#include "GraphVertex.hpp"
#include "GraphNode.hpp"
#include "AdjacencyListGraph.hpp"


using std::vector;


AdjacencyListGraph::AdjacencyListGraph() {}


AdjacencyListGraph::~AdjacencyListGraph()
{
    for (auto *node : this->nodes)
	delete node;
}


void AdjacencyListGraph::addVertex(GraphVertex &v)
{
    if (this->nodeMap.find(&v) == this->nodeMap.end()) {
	this->vertices.push_back(&v);
	GraphNode *n = new GraphNode(v);
	this->nodes.push_back(n);
	this->nodeMap[newVertex] = n;
    }
}


void AdjacencyListGraph::addEdge(GraphVertex &v1, GraphVertex &v2)
{
    this->nodeMap[&v1].addDownstreamNeighbor(v2);
    this->nodeMap[&v2].addUpstreamNeighbor(v1);
}


bool AdjacencyListGraph::edgeExists(GraphVertex &v1, GraphVertex &v2)
{
    for (auto *vertex : this->nodeMap[&v1].getDownstreamNeighbors()) 
	if (*vertex == v2)
	    return true;
    return false;
}


vector<GraphVertex*>& getDownstreamNeighbors(GraphVertex &v)
{
    return this->nodeMap[&v].getDownstreamNeighbors();
}


vector<GraphVertex*>& getUpstreamNeighbors(GraphVertex &v)
{
    return this->nodeMap[&v].getUpstreamNeighbors();
}


vector<GraphVertex*>& getVertices()
{
    return this->vertices;
}

