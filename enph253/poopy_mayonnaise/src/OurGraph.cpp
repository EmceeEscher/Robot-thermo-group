///
// OurGraph.cpp
//

#include <StandardCplusplus.h>
#include "OurGraph.hpp"

const int NUM_VERTICES = 20;

OurGraph::OurGraph()
  : numVertices(NUM_VERTICES){
    populateGraph();
  //GraphVertex one = new GraphVertex::GraphVertex(1, [0,0,0,2]);
}

OurGraph::~OurGraph() {
      for (int i(0); i < this->numVertices; ++i)
          delete nodes[i];
      delete[] nodes;
}

void OurGraph::populateGraph(){
  int zeroNeighbors [4] = {-1,-1,-1,19};
  GraphVertex *zero = new GraphVertex(0, zeroNeighbors);
  this->nodes[0] = zero;

  int oneNeighbors [4] = {-1,-1,-1,2};
  GraphVertex *one = new GraphVertex(1, oneNeighbors);
  this->nodes[1] = one;

  int twoNeighbors [4] = {-1,1,7,3};
  GraphVertex *two = new GraphVertex(2, twoNeighbors);
  this->nodes[2] = two;

  int threeNeighbors [4] = {-1,2,8,-1};
  GraphVertex *three = new GraphVertex(3, threeNeighbors);
  this->nodes[3] = three;

  int fourNeighbors [4] = {-1,-1,-1,6};
  GraphVertex *four = new GraphVertex(4, fourNeighbors);
  this->nodes[4] = four;

  int fiveNeighbors [4] = {-1,-1,6,-1};
  GraphVertex *five = new GraphVertex(5, fiveNeighbors);
  this->nodes[5] = five;

  int sixNeighbors [4] = {5,4,-1,7};
  GraphVertex *six = new GraphVertex(6, sixNeighbors);
  this->nodes[6] = six;

  int sevenNeighbors [4] = {2,6,7,8};
  GraphVertex *seven = new GraphVertex(7, sevenNeighbors);
  this->nodes[7] = seven;

  int eightNeighbors [4] = {3,7,17,-1};
  GraphVertex *eight = new GraphVertex(8, eightNeighbors);
  this->nodes[8] = eight;

  //nine,ten, and twelve might be fucked
  int nineNeighbors [4] = {7,10,12,-1};
  GraphVertex *nine = new GraphVertex(9, nineNeighbors);
  this->nodes[9] = nine;

  int tenNeighbors [4] = {9,11,12,-1};
  GraphVertex *ten = new GraphVertex(10, tenNeighbors);
  this->nodes[10] = ten;

  int elevenNeighbors [4] = {-1,-1,-1,10};
  GraphVertex *eleven = new GraphVertex(11, elevenNeighbors);
  this->nodes[11] = eleven;

  int twelveNeighbors [4] = {-1,10,13,9};
  GraphVertex *twelve = new GraphVertex(12, twelveNeighbors);
  this->nodes[12] = twelve;

  int thirteenNeighbors [4] = {12,14,19,17};
  GraphVertex *thirteen = new GraphVertex(13, thirteenNeighbors);
  this->nodes[13] = thirteen;

  int fourteenNeighbors [4] = {-1,16,15,13};
  GraphVertex *fourteen = new GraphVertex(14, fourteenNeighbors);
  this->nodes[14] = fourteen;

  int fifteenNeighbors [4] = {14,-1,-1,-1};
  GraphVertex *fifteen = new GraphVertex(15, fifteenNeighbors);
  this->nodes[15] = fifteen;

  int sixteenNeighbors [4] = {-1,-1,-1,14};
  GraphVertex *sixteen = new GraphVertex(16, sixteenNeighbors);
  this->nodes[16] = sixteen;

  int seventeenNeighbors [4] = {8,13,18,-1};
  GraphVertex *seventeen = new GraphVertex(17, seventeenNeighbors);
  this->nodes[17] = seventeen;

  int eighteenNeighbors [4] = {17,19,-1,-1};
  GraphVertex *eighteen = new GraphVertex(18, eighteenNeighbors);
  this->nodes[18] = eighteen;

  int nineteenNeighbors [4] = {13,0,-1,18};
  GraphVertex *nineteen = new GraphVertex(19, nineteenNeighbors);
  this->nodes[19] = nineteen;
}

