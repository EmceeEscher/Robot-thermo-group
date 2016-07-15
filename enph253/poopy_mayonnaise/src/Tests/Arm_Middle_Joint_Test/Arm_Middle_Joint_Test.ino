#include <phys253.h>
#include <phys253pins.h>

int val = 0;
 
void setup()
{
    #include <phys253setup.txt>
    Serial.begin(9600);
    if(RCServo0.attached()){
      LCD.print("FUCKFUCK");
      delay(2000);
    }
}

void loop()
{
  LCD.clear();
  int val = knob(6);
  
  int angle = map(val, 0, 1023, 0, 180);
  
  LCD.setCursor(0,0);
  LCD.print("angle = ");
  LCD.print(angle); 
  RCServo0.write(angle);
  delay(200);
}
