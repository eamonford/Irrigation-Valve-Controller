
#include "CommandProcessor.h"
#include "Debug.h"
#include <AltSoftSerial.h>

#define VALVE_CONTROL_PIN A3

AltSoftSerial backPort;
CommandProcessor* commandProcessor;

void setup() {
  Serial.begin(9600);
  backPort.begin(9600);
  #if DEBUG == true
    // If in DEBUG mode, treat the real RX and TX pins as going toward Master because they are the same as the USB port
    commandProcessor = new CommandProcessor(VALVE_CONTROL_PIN, &Serial, &backPort);
  #else
    commandProcessor = new CommandProcessor(VALVE_CONTROL_PIN, &backPort, &Serial);
  #endif
}

// Won't speak unless spoken to.
void loop() {
  while (true) {
    commandProcessor->getAndProcessDatagram();
  }
}
