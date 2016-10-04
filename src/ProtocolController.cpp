#include "ProtocolController.h"
#include "constants.h"

ProtocolController(BoardController* boardController, Stream* frontComm, Stream* backComm) {
    this->boardController = boardController;
    this->frontComm = frontComm;
    this->backComm = backComm;
}

bool ProtocolController::sendMessage(Message* msg, int identity) {
  // if (DEBUG) { Serial.println("sendMessage()"); }
  if (identity != 0x00 && msg->destination == identity)
    return true;
  else if (msg->destination == MASTER) {
    // if (DEBUG) { Serial.println("Message is for MASTER"); }
    activePort = backPort;
  } else {
    // if (DEBUG) { Serial.println("Message is not for MASTER"); }
    activePort = &((AltSoftSerial&)Serial);
  }
  if (sendSynAndWaitForAck()) {
    activePort->write(msg->destination);
    activePort->write(msg->command);
    activePort->write(msg->arg);
    return true;
  }
  return false;
}

bool ProtocolController::sendSynAndWaitForAck() {
  int tryCount = 0;
  do {
    activePort->write(SYN);
    delay(100);
    if (tryCount++ == 30) {
      return false;
    }
  } while (activePort->available() == 0 || activePort->read() != ACK);
  return true;
}

void ProtocolController::waitForSynAndSendAck() {
  if (DEBUG) { Serial.println("waitForSynAndSendAck()"); }
  bool forward = true;
  do {
    if (forward) {
      if (DEBUG) { Serial.println("Opening forward serial port"); }
      activePort = frontPort;
    } else {
      if (DEBUG) { Serial.println("Opening backward serial port"); }
      activePort = backPort;
    }
    forward = !forward;
    delay(300);
  } while (activePort->available() == 0 || activePort->read() != SYN);

  if (DEBUG) { Serial.println("Received SYN, now sending ACK"); }
  activePort->write(ACK);
  // Eat up any extra SYNs that got sent during the delay
  while (activePort->available() > 0 && activePort->peek() == SYN) {
    activePort->read();
  }
}

int* ProtocolController::readBytes(int numBytesToRead) {
  int* message = (int*)malloc(sizeof(int)*numBytesToRead);
  int numBytesReceived = 0;
  while (numBytesReceived < numBytesToRead) {
    if (activePort->available() > 0) {
      message[numBytesReceived++] = activePort->read();
    }
  }
  return message;
}

Message* ProtocolController::waitAndGetMessage() {
    waitForSynAndSendAck();
    int* messageBytes = readBytes(MAX_MSG_LEN);
    return Message::parse(messageBytes);
}
