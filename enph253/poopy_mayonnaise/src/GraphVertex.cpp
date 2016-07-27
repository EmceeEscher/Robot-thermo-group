///
// GraphVertex.cpp
//
#include <iostream>
#include <string>
#include "GraphVertex.hpp"


using std::string;
using std::ostream;


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


GraphVertex::operator string() const
{
    return this->label;
}


bool operator==(const GraphVertex &me, const GraphVertex &other)
{
    return me.label == other.label;
}


bool operator!=(const GraphVertex &me, const GraphVertex &other)
{
    return !(me == other);
}


ostream& operator<<(ostream &out, const GraphVertex &vertex)
{
    out << string(vertex);
    return out;
}
