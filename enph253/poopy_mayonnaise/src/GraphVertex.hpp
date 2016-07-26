///
// GraphVertex.hpp
//
// Definition of a graph vertex; essentially a port of that used by the
// CPEN 221 staff for the graph definition in Machine Problem 3
//
#ifndef GRAPH_VERTEX_HPP
#define GRAPH_VERTEX_HPP

#include <string>

using std::ostream;
using std::string;

class GraphVertex
{

private:

    string label;

public:

    GraphVertex(const string label);  // constructor

    /*
     * Returns a reference to the vertex's label string
     */
    string& getLabel();

    /*
     * Set the vertex's label to the given value
     */
    void setLabel(string label);

    /*
     * Overload the string operator to give nice string representation.
     */
    operator string();

    /*
     * Overload equality comparison operator to return true if labels are equal
     */
    friend bool operator==(const GraphVertex &me, const GraphVertex &other);

    /*
     * Overload innequality comparison operator to reflect == operator
     */
    friend bool operator!=(const GraphVertex &me, const GraphVertex &other);

    /*
     * Overload << operator to produce nice output
     */
    friend ostream& operator<<(const ostream &out, const GraphVertex &vertex);

};

#endif  // GRAPH_VERTEX_HPP
