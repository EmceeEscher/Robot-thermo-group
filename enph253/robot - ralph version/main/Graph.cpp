int cardinalDirection = 2;
int nextCardinalDirection;
int previousNode = 1;
int currentNode = 2;
int nextNode;
int graphTurnDirection;
int isClockwise = -1;

int graph[20][4]{
  //N,  E,  S,  W
  {-1, -1, 19, -1}, //0
  {-1, -1,  2, -1}, //1
  { 1,  7,  3, -1}, //2
  { 2,  8, -1, -1}, //3
  {-1, -1,  6, -1}, //4
  {-1,  6, -1, -1}, //5
  { 4, -1,  7,  2}, //6
  { 6,  9,  8,  2}, //7
  {-1, -1, -1, -1}, //8
  {10, -1, 12, -1}, //9
  {11, -1, 12, 9}, //10
  {-1, -1, 10, -1}, //11
  {10, 13,  9, -1}, //12
  {14, 19, 17, 12}, //13
  {16, 15, -1, -1}, //14
  {-1, -1, -1, 14}, //15
  {-1, -1, 14, -1}, //16
  {13, 18, -1,  8}, //17
  {19, -1, -1, -1}, //18
  { 0, -1, 18, 13}, //19
  };

int getNextCardinalDirection(int currNode){
  int randomDirection = 0;// make Random
  
  while (graph[currNode][randomDirection] == -1){
    randomDirection = 0; //make Random
  }
  
  return randomDirection;
}

int getNextNode(int currNode, int nextDir){
  int next = graph[currNode][nextDir];
  return next;
}

int getTurnDirection(int cardinalDir, int nextCardinalDir){
  if (cardinalDir == 0 && nextCardinalDir == 3){
    return -1;
  }
  else{
    return nextCardinalDir - cardinalDir;
  }
}

void graphLoop(){
  if (isClockwise == -1 && (currentNode == 9 || currentNode == 10 || currentNode == 12)){
    if (graphTurnDirection == -1)
      isClockwise = 1;
    else if (graphTurnDirection == 1)
      isClockwise = 0;
  }
  
  nextCardinalDirection = getNextCardinalDirection(currentNode);
  nextNode = getNextNode(currentNode, nextCardinalDirection);
  graphTurnDirection = getTurnDirection(cardinalDirection, nextCardinalDirection);
  
  if (isClockwise != -1 && (nextNode == 9 || nextNode == 10 || nextNode == 12)){
    if (isClockwise = 1)
      nextCardinalDirection = nextCardinalDirection + 1;
    else
      nextCardinalDirection = nextCardinalDirection - 1; 
  }
  else
    isClockwise = -1;
  
  cardinalDirection = nextCardinalDirection;
  previousNode = currentNode;
  currentNode = nextNode;
}


