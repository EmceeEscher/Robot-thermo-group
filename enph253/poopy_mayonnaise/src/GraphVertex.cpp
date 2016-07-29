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


ostream& operator<<(ostream &out, const GraphVertex &vertex)
{
    out << string(vertex);
    return out;
}


bool operator<(const GraphVertex &v1, const GraphVertex &v2)
{
    return v1.label < v2.label;
}


bool operator==(const GraphVertex &v1, const GraphVertex &v2)
{
    return !((v1 < v2) || (v2 < v1));
}


bool operator!=(const GraphVertex &v1, const GraphVertex &v2)
{
    return !(v1 == v2);
}
