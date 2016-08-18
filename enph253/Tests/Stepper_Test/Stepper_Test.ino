#include <phys253.h>          
#include <LiquidCrystal.h>    


int value;
double pulses;
const double ratio = 250.0/1024.0;
unsigned long prevTime;
unsigned long milliTime;
const int pulsePin = 9; 
const int delayPeriod = 7000; //Microseconds

void setup()
{
    #include <phys253setup.txt>
    Serial.begin(9600);
    portMode(1,OUTPUT);
    pinMode(pulsePin,OUTPUT);
    prevTime = millis();
}

   
 
void loop()
{
  milliTime = millis();
  value = knob(6);
  pulses = (double) value * ratio;
  LCD.clear();
  LCD.print(pulses);
  if(startbutton()){
    LCD.clear();
    LCD.print("Pulsing");
    for(int i = 0; i < (int) pulses; i++){
      digitalWrite(pulsePin,HIGH);
      delayMicroseconds(delayPeriod);
      digitalWrite(pulsePin,LOW);
      delayMicroseconds(delayPeriod);
    }
    prevTime = millis();
  }
  delay(50);
}
