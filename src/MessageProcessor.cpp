#ifndef UNIT_TEST

#include "MessageProcessor.h"
#include "constants.h"

MessageProcessor::MessageProcessor(HardwareController* hardwareController) {
    this->hardwareController = hardwareController;
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

  int id = hardwareController->hasId() ? hardwareController->getId() : 0x00;
  if (message->isForId(id)) {
    executeCommand(message, id);
  }

  if (message->isForIdOtherThan(id)) {
    if (!hardwareController->sendMessage(message, id) && message->destination != MASTER) {
      hardwareController->sendMessage(new Message(MASTER, END_OF_CHAIN, id), id);
    }
  }
}

void MessageProcessor::executeCommand(Message* message, int id) {
  if (DEBUG) { Serial.println("executeCommand()"); }
  switch (message->command) {
      case SET_ID:
        hardwareController->sendMessage(new Message(MASTER,
                                0x01,
                                hardwareController->setId(message->arg)?message->arg : 0x00),
                    id);
        break;
      case OPEN_VALVE:
        hardwareController->openValve();
        hardwareController->sendMessage(new Message(MASTER, OPEN_VALVE, id), id);
        break;
      case CLOSE_VALVE:
      hardwareController->closeValve();
        hardwareController->sendMessage(new Message(MASTER, CLOSE_VALVE, id), id);
        break;
      case IDENTIFY:
          hardwareController->sendMessage(new Message(MASTER, 0x01, id), id);
        break;
      default:
        hardwareController->sendMessage(new Message(MASTER, 0x00, 0x00), id);
    }
}

#endif
