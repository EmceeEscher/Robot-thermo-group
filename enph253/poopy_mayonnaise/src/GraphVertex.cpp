///
// GraphVertex.cpp
//
#include <string>
#include "GraphVertex.hpp"


using std::string;


GraphVertex::GraphVertex(const string label)
    : label(label)
{}


string& GraphVertex::getLabel()
{
    return this->label;
}


void GraphVertex::setLabel(const string label)
{
    this->label = label;
}
