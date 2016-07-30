///
// GraphVertex.cpp
//

#include <StandardCplusplus.h>
#include "GraphVertex.hpp"

const int NEIGHBORS_SIZE {4};

GraphVertex::GraphVertex(int num, int* neighbors)
  : num(num),
    neighbors(neighbors){  
}

int GraphVertex::getNum(){
  return this->num;
}

int* GraphVertex::getNeighbors(){
  return this->neighbors;
}

