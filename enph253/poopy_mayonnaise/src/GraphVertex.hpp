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
    int* neighbors;

public:

    GraphVertex(const string label, int neighbors[]);  // constructor

    /*
     * Returns a reference to the vertex's label string
     */
    string& getLabel();

    /*
     * Set the vertex's label to the given value
     */
    void setLabel(const string label);

    /*
     * Returns a 4-dimensional array of the vertex's neighbors. Each index corresponds to a direction,
     * 0 = west, 1 = north, 2 = east, 3 = south
     * Each element of the array will be the number of the vertex that is connected, or 
     * 0 if there is no edge in that direction.
     */
    int* getNeighbors();

//    /*
//     * Overload the string operator to give nice string representation.
//     */
//    operator string() const;
//
//    /*
//     * Overload << operator to produce nice output
//     */
//    friend ostream& operator<<(ostream &out, const GraphVertex &vertex);
//
//    /*
//     * Less comparison operator for ordering elements (e.g. in a map)
//     */
//    friend bool operator<(const GraphVertex &v1, const GraphVertex &v2);
//
//    /*
//     * Overload equality comparison operator to return true if labels are equal
//     */
//    friend bool operator==(const GraphVertex &v1, const GraphVertex &v2);
//
//    /*
//     * Overload innequality comparison operator to reflect == operator
//     */
//    friend bool operator!=(const GraphVertex &v1, const GraphVertex &v2);

};

#endif  // GRAPH_VERTEX_HPP
