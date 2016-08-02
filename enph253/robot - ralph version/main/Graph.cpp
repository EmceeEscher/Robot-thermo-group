///
// Graph.cpp
//
#include <phys253.h>
#include "Graph.hpp"


Direction graphTurnDirection;

static int cardinalDirection = 2;
static int nextCardinalDirection;
static int previousNode = 1;
static int currentNode = 2;
static int nextNode;
static int isClockwise = -1;

static int graph[20][4] {
    // N,  E,  S,  W
    { -1, -1, 19, -1 }, // 0
    { -1, -1,  2, -1 }, // 1
    {  1,  7,  3, -1 }, // 2
    {  2,  8, -1, -1 }, // 3
    { -1, -1,  6, -1 }, // 4
    { -1,  6, -1, -1 }, // 5
    {  4, -1,  7,  2 }, // 6
    {  6,  9,  8,  2 }, // 7
    {  7, 17, -1,  3 }, // 8
    { 10, -1, 12, -1 }, // 9
    { 11, -1, 12,  9 }, // 10
    { -1, -1, 10, -1 }, // 11
    { 10, 13,  9, -1 }, // 12
    { 14, 19, 17, 12 }, // 13
    { 16, 15, -1, -1 }, // 14
    { -1, -1, -1, 14 }, // 15
    { -1, -1, 14, -1 }, // 16
    { 13, 18, -1,  8 }, // 17
    { 19, -1, -1, -1 }, // 18
    {  0, -1, 18, 13 }, // 19
};


int getNextCardinalDirection(int currNode)
{
    int randomDirection = random(4);
    while (graph[currNode][randomDirection] == -1
           && graph[currNode][randomDirection] != previousNode) {
        randomDirection = random(4);
    }
    return randomDirection;
}


int getNextNode(int currNode, int nextDir)
{
    return graph[currNode][nextDir];
}


Direction getTurnDirection(int cardinalDir, int nextCardinalDir) {
    if (cardinalDir == 0 && nextCardinalDir == 3) 
        return Direction::LEFT;
    else
        return static_cast<Direction>(nextCardinalDir - cardinalDir + 1);
}


void graphLoop()
{
    if (isClockwise == -1 &&
        (currentNode == 9 || currentNode == 10 || currentNode == 12)) {
        if (graphTurnDirection == Direction::LEFT)
            isClockwise = 1;
        else if (graphTurnDirection == Direction::RIGHT)
            isClockwise = 0;
    }
  
    nextCardinalDirection = getNextCardinalDirection(currentNode);
    nextNode = getNextNode(currentNode, nextCardinalDirection);
    graphTurnDirection = getTurnDirection(cardinalDirection, nextCardinalDirection);
    
    Serial.print( F("CD: ") );
    Serial.print(cardinalDirection);
    Serial.print( F(" to ") ); 
    
    if (isClockwise != -1 && (nextNode == 9 || nextNode == 10 || nextNode == 12)) {
        if (isClockwise == 1) {
            cardinalDirection = nextCardinalDirection + 1;
            if (cardinalDirection > 3)
                cardinalDirection = 0;
        } else {
            nextCardinalDirection = nextCardinalDirection - 1; 
            if (cardinalDirection < 0)
                cardinalDirection = 3;
        }
    } else
        isClockwise = -1;
    cardinalDirection = nextCardinalDirection;
    
    Serial.println(nextCardinalDirection);
    Serial.print( F("node: ") );
    Serial.print(previousNode);
    Serial.print( F(" -> ") );
    Serial.print(currentNode);
    Serial.print( F(" -> ") );
    Serial.println(nextNode);
    
    previousNode = currentNode;
    currentNode = nextNode;
}


void graphLoop(int setNode)
{
    int setCardinalDirection = -1;

    for (int i(0); i < 4; ++i) {
        if (graph[currentNode][i] == setNode) {
            setCardinalDirection = i;
            break;
        }
    }
    
    if (setCardinalDirection == -1) {
        graphLoop();
        return;
    }
    
    if (isClockwise == -1 &&
        (currentNode == 9 || currentNode == 10 || currentNode == 12)) {
        if (graphTurnDirection == Direction::LEFT)
            isClockwise = 1;
        else if (graphTurnDirection == Direction::RIGHT)
            isClockwise = 0;
    }
    
    graphTurnDirection = getTurnDirection(setNode, setCardinalDirection);
    
    if (isClockwise != -1 && (nextNode == 9 || nextNode == 10 || nextNode == 12)) {
        if (isClockwise == 1) {
            cardinalDirection = setCardinalDirection + 1;
            if (cardinalDirection > 3)
                cardinalDirection = 0;
        } else {
            cardinalDirection = setCardinalDirection - 1;
            if (cardinalDirection < 0)
                cardinalDirection = 3;
        }
    } else {
        isClockwise = -1;
        cardinalDirection = setCardinalDirection;
    }
    
    previousNode = currentNode;
    currentNode = setNode;
}
