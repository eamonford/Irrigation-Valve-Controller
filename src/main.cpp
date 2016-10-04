#ifndef UNIT_TEST

#include <Arduino.h>
#include <AltSoftSerial.h>
#include <Message.h>
#include <avr/io.h>
#include "constants.h"
#include "MessageProcessor.h"
#include "HardwareController.cpp"

HardwareController* hardwareController;
MessageProcessor* messageProcessor;

void setup() {
    AltSoftSerial backPort;
    Serial.begin(9600);
    backPort.begin(9600);
    pinMode(VALVE_CONTROL, OUTPUT);
    hardwareController = new HardwareController(VALVE_CONTROL, (AltSoftSerial*)(&Serial), &backPort);
    messageProcessor = new MessageProcessor(hardwareController);
}

// Won't speak unless spoken to.
void loop() {
  while (true) {
      Message* message = hardwareController->waitAndGetMessage();
      messageProcessor->processMessage(message);

    // interface.waitForSynAndSendAck();
    // int* messageBytes = interface.readBytes(MAX_MSG_LEN);
    // Message* message = Message::parse(messageBytes);
    // free(messageBytes);
    // processMessage(message);
    // delete message;
  }
}

#endif
