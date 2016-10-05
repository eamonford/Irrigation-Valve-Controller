#ifndef UNIT_TEST

#include <Arduino.h>
#include <AltSoftSerial.h>
#include <Message.h>
#include <avr/io.h>
#include "constants.h"
#include "ProtocolController.h"
#include "BoardController.h"

BoardController* boardController;
ProtocolController* protocolController;

void setup() {
    AltSoftSerial backPort;
    Serial.begin(9600);
    backPort.begin(9600);
    pinMode(VALVE_CONTROL, OUTPUT);
    boardController = new BoardController(VALVE_CONTROL);
    protocolController = new ProtocolController(&Serial, &backPort);
}

void executeCommand(Message* message, int id) {
  if (DEBUG) { Serial.println("executeCommand()"); }
  switch (message->command) {
      case SET_ID:
        protocolController->sendMessage(new Message(MASTER,
                                0x01,
                                boardController->setId(message->arg)?message->arg : 0x00),
                    id);
        break;
      case OPEN_VALVE:
        boardController->openValve();
        protocolController->sendMessage(new Message(MASTER, OPEN_VALVE, id), id);
        break;
      case CLOSE_VALVE:
      boardController->closeValve();
        protocolController->sendMessage(new Message(MASTER, CLOSE_VALVE, id), id);
        break;
      case IDENTIFY:
          protocolController->sendMessage(new Message(MASTER, 0x01, id), id);
        break;
      default:
        protocolController->sendMessage(new Message(MASTER, 0x00, 0x00), id);
    }
}

void processMessage(Message* message) {
  if (DEBUG) { Serial.println("processMessage()"); }
  if (DEBUG) {
    Serial.println("Processing message:");
    Serial.write(message->destination);
    Serial.write(message->command);
    Serial.write(message->arg);
    Serial.println("End of message dump");
  }

  int id = boardController->hasId() ? boardController->getId() : 0x00;
  if (message->isForId(id)) {
    executeCommand(message, id);
  }

  if (message->isForIdOtherThan(id)) {
    if (!protocolController->sendMessage(message, id) && message->destination != MASTER) {
      protocolController->sendMessage(new Message(MASTER, END_OF_CHAIN, id), id);
    }
  }
}

// Won't speak unless spoken to.
void loop() {
  while (true) {
    Stream* comm = protocolController->waitForSynAndSendAck();
    int* messageBytes = protocolController->readBytes(comm, MAX_MSG_LEN);
    Message* message = Message::parse(messageBytes);
    free(messageBytes);
    processMessage(message);
    delete message;
  }
}

#endif
