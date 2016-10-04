#ifndef UNIT_TEST

#include "MessageProcessor.h"
#include "constants.h"

MessageProcessor::MessageProcessor(ProtocolController* protocolController) {
    this->protocolController = protocolController;
}

void MessageProcessor::processMessage(Message* message) {
  if (DEBUG) { Serial.println("processMessage()"); }
  if (DEBUG) {
    Serial.println("Processing message:");
    Serial.write(message->destination);
    Serial.write(message->command);
    Serial.write(message->arg);
    Serial.println("End of message dump");
  }

  int id = protocolController->boardController->hasId() ? protocolController->boardController->getId() : 0x00;
  if (message->isForId(id)) {
    executeCommand(message, id);
  }

  if (message->isForIdOtherThan(id)) {
    if (!protocolController->sendMessage(message, id) && message->destination != MASTER) {
      protocolController->sendMessage(new Message(MASTER, END_OF_CHAIN, id), id);
    }
  }
}

void MessageProcessor::executeCommand(Message* message, int id) {
  if (DEBUG) { Serial.println("executeCommand()"); }
  switch (message->command) {
      case SET_ID:
        protocolController->sendMessage(new Message(MASTER,
                                0x01,
                                protocolController->boardController->setId(message->arg)?message->arg : 0x00),
                    id);
        break;
      case OPEN_VALVE:
        protocolController->boardController->openValve();
        protocolController->sendMessage(new Message(MASTER, OPEN_VALVE, id), id);
        break;
      case CLOSE_VALVE:
      protocolController->boardController->closeValve();
        protocolController->sendMessage(new Message(MASTER, CLOSE_VALVE, id), id);
        break;
      case IDENTIFY:
          protocolController->sendMessage(new Message(MASTER, 0x01, id), id);
        break;
      default:
        protocolController->sendMessage(new Message(MASTER, 0x00, 0x00), id);
    }
}

#endif
