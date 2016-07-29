///
// GraphNode.cpp
//
#include "GraphNode.hpp"


GraphNode::GraphNode(GraphVertex& v) : self(v) {}


void GraphNode::addUpstreamNeighbor(GraphVertex &v)
{
    this->upstreamNeighbors.push_back(&v);
}


void GraphNode::addDownstreamNeighbor(GraphVertex &v)
{
    this->downstreamNeighbors.push_back(&v);
}


vector<GraphVertex*>& GraphNode::getUpstreamNeighbors()
{
    return this->upstreamNeighbors;
}


vector<GraphVertex*>& GraphNode::getDownstreamNeighbors()
{
    return this->downstreamNeighbors;
}


GraphVertex& GraphNode::getNodeVertex()
{
    return this->self;
}
