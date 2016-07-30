#include <phys253.h>
#include <phys253pins.h>

void setup() {
  #include <phys253setup.txt>

}

void loop() {
  int leftReading = analogRead(0);
  int rightReading = analogRead(1);

  LCD.clear();
  LCD.print("left: ");
  LCD.print(leftReading);
  LCD.setCursor(0,1);
  LCD.print("right: ");
  LCD.print(rightReading);

  delay(500);

}
