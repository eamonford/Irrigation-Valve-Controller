
#include "CommandProcessor.h"
#include <AltSoftSerial.h>

/*
void flashLED(int milli) {
  digitalWrite(LED_BUILTIN, HIGH);
  delay(milli);
  digitalWrite(LED_BUILTIN, LOW);
  delay(milli);
}

void holdLEDOn(int milli) {
  digitalWrite(LED_BUILTIN, HIGH);
  delay(milli);
  digitalWrite(LED_BUILTIN, LOW);
}

void flashDatagram(Datagram* datagram) {
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
}*/

#define VALVE_CONTROL_PIN A3

AltSoftSerial backPort;
CommandProcessor* commandProcessor;

void setup() {
  Serial.begin(9600);
  backPort.begin(9600);
  commandProcessor = new CommandProcessor(VALVE_CONTROL_PIN, &backPort, &Serial);
}

// Won't speak unless spoken to.
void loop() {
  while (true) {
    commandProcessor->getAndProcessDatagram();
  }
}
