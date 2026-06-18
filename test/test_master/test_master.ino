//for testing functionality, uses a larger master device (not the STM31L011)
#include <Wire.h>

#define LED_PIN 33

void setup()
{
  pinMode(LED_PIN, OUTPUT);
  pinMode(36, INPUT_PULLUP);
  pinMode(37, INPUT_PULLUP);
  Wire.setSDA(36);
  Wire.setSCL(37);
  Wire.begin(); // join i2c bus (address optional for master)
  Serial.begin(115200);
  Serial.print("starting");
}

byte x = 0;

void loop()
{
  Wire.beginTransmission(37); // transmit to device 0x25
  Wire.write(x);              // sends one byte  
  Wire.endTransmission();    // stop transmitting
  Serial.printf("sent");
  delay(500);
  Wire.requestFrom(37, 1);
  while (Wire.available()) {
    x=Wire.read();
    Serial.printf("value = %d\n",x);
  }
  digitalWrite(LED_PIN, !digitalRead(LED_PIN));

  delay(500);
}
