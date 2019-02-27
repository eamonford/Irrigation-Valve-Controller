
#include "CommandProcessor.h"
#include "Debug.h"
#include <AltSoftSerial.h>

#define VALVE_CONTROL_PIN A3

AltSoftSerial backPort;
CommandProcessor* commandProcessor;

void setup() {
  Serial.begin(9600);
  backPort.begin(9600);
  commandProcessor = new CommandProcessor(VALVE_CONTROL_PIN, &Serial, &backPort);
}

// Won't speak unless spoken to.
void loop() {
  while (true) {
    commandProcessor->getAndProcessDatagram();
  }
}
