#include <phys253.h>
#include <phys253pins.h>

float leftWeight;
float rightWeight;
float straightWeight;

void setup() {
  #include <phys253setup.txt>
  Serial.begin(9600);
  // put your setup code here, to run once:
  leftWeight = 0.;
  rightWeight = 0.;
  straightWeight = 0.;
}

void loop() {
  // put your main code here, to run repeatedly:
  int leftInput;
  int rightInput;
  int straightInput;
  bool leftBool = true;
  bool rightBool = true;
  bool straightBool = true;
  while(!stopbutton()){
    leftInput = analogRead(6);
    LCD.clear();
    LCD.print("leftInput: ");
    LCD.print(leftInput);
    delay(50);
  }
  delay(500);
  while(!stopbutton()){
    rightInput = analogRead(6);
    LCD.clear();
    LCD.print("rightInput: ");
    LCD.print(rightInput);
    delay(50);
  }
  delay(500);
  while(!stopbutton()){
    straightInput = analogRead(6);
    LCD.clear();
    LCD.print("straightI: ");
    LCD.print(straightInput);
    delay(50);
  }
  delay(500);
  while(!stopbutton()){
    if(startbutton()){
      leftBool = !leftBool;
    }
    LCD.clear();
    LCD.print("leftBool: ");
    LCD.print(leftBool);
    delay(50);
  }
  delay(500);
  while(!stopbutton()){
    if(startbutton()){
      rightBool = !rightBool;
    }
    LCD.clear();
    LCD.print("rightBool: ");
    LCD.print(rightBool);
    delay(50);
  }
  delay(500);
  while(!stopbutton()){
    if(startbutton()){
      straightBool = !straightBool;
    }
    LCD.clear();
    LCD.print("straightB: ");
    LCD.print(straightBool);
    delay(50);
  }
  Serial.print("leftInput: ");
  Serial.println(leftInput);
  Serial.print("rightInput: ");
  Serial.println(rightInput);
  Serial.print("straightInput: ");
  Serial.println(straightInput);
  Serial.print("leftBool: ");
  Serial.println(leftBool);
  Serial.print("rightBool: ");
  Serial.println(rightBool);
  Serial.print("straightBool: ");
  Serial.println(straightBool);
  giveTurnDirection(leftInput, rightInput, straightInput);
  String result = chooseTurn(leftBool, rightBool, straightBool);
  Serial.print("result: ");
  Serial.println(result);
}

// TODO: generalize
String chooseTurn(bool left, bool right, bool straight)
{
    // // for now, random
    // if (left && right && straight)
    //   return random(3) - 1;
    // else if (left && right)
    //  return 2*random(2) - 1;
    // else if (left && straight)
    //  return random(2) - 1;
    // else if (right && straight)
    //  return random(2);
    // else if (left)
    //  return -1;
    // else if (right)
    //  return 1;
    // else
    //  return 0;

    // for now, prefer left, then right, then straight
    /*if (left)
      return Direction::LEFT;  // left
    else if (right)
      return Direction::RIGHT;   // right
    else
      return Direction::FRONT;   // straight*/

    float total = left*leftWeight + right*rightWeight + straight*straightWeight;
    float leftProb;
    float rightProb;
    float straightProb;
    if(abs(total)<0.0001){
      leftProb = left*100./(left+right+straight);
      rightProb = right*100./(left+right+straight);
      straightProb = straight*100./(left+right+straight);
    }else{
      leftProb = (left*leftWeight)/total * 100;
      rightProb = (right*rightWeight)/total * 100;
      straightProb = (straight*straightWeight)/total * 100;
    }

   
    
    int randValue = rand() % 100;
    float leftMax = 0 + leftProb;
    float rightMax = leftProb + rightProb;
    float straightMax = 100;

    Serial.print("Total: ");
    Serial.println(total);
    Serial.print("leftProb: ");
    Serial.println(leftProb);
    Serial.print("rightProb: ");
    Serial.println(rightProb);
    Serial.print("straightProb: ");
    Serial.println(straightProb);
    Serial.print("randValue: ");
    Serial.println(randValue);
      
    String desired;
    if(randValue < leftMax){
      desired = "left";
    }else if(randValue < rightMax){
      desired = "right";
    }else{
      desired = "straight";
    }
   
    /*Direction returnVal;
    if(desired == Direction::LEFT){
      if(left){
        returnVal = Direction::LEFT;
      }else{
        float newTotal = (right*rightWeight) + straightWeight;
        float newRightWeight = rightWeight/newTotal*100;
        float newStraightWeight = straightWeight/newTotal*100;
        float newRightMax = newRightWeight;
        if(randValue < newRightMax){
          if(right){
            returnVal =  Direction::RIGHT;
          }else{
            returnVal =  Direction::FRONT;
          }
        }else{
          if(straight){
            returnVal =  Direction::FRONT;
          }else{
            returnVal =  Direction::RIGHT;
          }
        }
      }
    }else if(desired == Direction::RIGHT){
      if(right){
        returnVal =  Direction::RIGHT;
      }else{
        float newTotal = leftWeight + straightWeight;
        float newLeftWeight = leftWeight/newTotal*100;
        float newStraightWeight = straightWeight/newTotal*100;
        float newLeftMax = newLeftWeight;
        if(randValue < newLeftMax){
          if(left){
            returnVal =  Direction::LEFT;
          }else{
            returnVal =  Direction::FRONT;
          }
        }else{
          if(straight){
            returnVal =  Direction::FRONT;
          }else{
            returnVal =  Direction::LEFT;
          }
        }
      }
    }else if(desired == Direction::FRONT){
      if(straight){
        returnVal =  Direction::FRONT;
      }else{
        float newTotal = rightWeight + leftWeight;
        float newRightWeight = rightWeight/newTotal*100;
        float newLeftWeight = leftWeight/newTotal*100;
        float newRightMax = newRightWeight;
        if(randValue < newRightMax){
          if(right){
            returnVal =  Direction::RIGHT;
          }else{
            returnVal =  Direction::LEFT;
          }
        }else{
          if(left){
            returnVal =  Direction::LEFT;
          }else{
            returnVal =  Direction::RIGHT;
          }
        }
      }
    }*/
    leftWeight = 0.;
    rightWeight = 0.;
    straightWeight = 0.;
    return desired;

}

void giveTurnDirection(int left, int right, int straight){
    leftWeight = (float) left;
    rightWeight = (float) right;
    straightWeight = (float) straight;
    /*float total = left+right+straight;
    if(total == 0){
      leftWeight = 100./3;
      rightWeight = 100./3;
      straightWeight = 100./3;
    }else{
      leftWeight = ((float)left)/total * 100;
      rightWeight = ((float)right)/total * 100;
      straightWeight = ((float)straight)/total * 100;
    }*/
}
