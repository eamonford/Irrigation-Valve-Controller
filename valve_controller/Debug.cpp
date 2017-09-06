#include "Debug.h"

void Debug::flashLED(int milli) {
  digitalWrite(LED_BUILTIN, HIGH);
  delay(milli);
  digitalWrite(LED_BUILTIN, LOW);
  delay(milli);
}

void Debug::holdLEDOn(int milli) {
  digitalWrite(LED_BUILTIN, HIGH);
  delay(milli);
  digitalWrite(LED_BUILTIN, LOW);
}

void Debug::flashDatagram(Datagram* datagram) {
  for (int i = 0; i < 20; i++) {
      flashLED(50);
  }
  for (int i = 0; i < datagram->destination; i++) {
    flashLED(500);
  }
  delay(1000);
  flashLED(100);
  flashLED(100);
  delay(1000);
  for (int i = 0; i < datagram->command; i++) {
    flashLED(500);
  }
  delay(1000);
  flashLED(100);
  flashLED(100);
  delay(1000);
  for (int i = 0; i < datagram->arg; i++) {
    flashLED(500);
  }
  holdLEDOn(2000);
  delay(2000);
}
