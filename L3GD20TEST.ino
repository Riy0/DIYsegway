#include "L3.h"

L3GD20 gyro;

void setup() {
  Serial.begin(9600);
  if(!gyro.begin(gyro.L3GD20_RANGE_250DPS)){
    Serial.println("Unable to initialize");
    while(true);
  }
}

void loop() {
  gyro.read();
  Serial.print("X: ");
  Serial.print((int)gyro.data.x);
  Serial.print(" ");

  Serial.print("Y: ");
  Serial.print((int)gyro.data.y);
  Serial.print(" ");

  Serial.print("Z: ");
  Serial.println((int)gyro.data.z);

  delay(50);
}
