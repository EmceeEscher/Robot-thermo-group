#include "C:\Users\rywais\Documents\Robot-thermo-group\enph253\poopy_mayonnaise\src\DetectBeacon.cpp"
#include "C:\Users\rywais\Documents\Robot-thermo-group\enph253\poopy_mayonnaise\src\MinorMode.cpp"

DetectBeacon* detector;

void setup() {
  detector = new DetectBeacon();  
}

void loop() {
  LCD.clear();
  LCD.print(detector->getBeaconDirection());
//  LCD.setCursor(0,1);
//  LCD.print(analogRead(6));
//  LCD.print(F("   "));
//  LCD.print(analogRead(7));
  delay(100);
}
