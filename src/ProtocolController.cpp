#include "ProtocolController.h"
#include "constants.h"

ProtocolController::ProtocolController(Stream* frontComm, Stream* backComm) {
    this->frontComm = frontComm;
    this->backComm = backComm;
}

bool ProtocolController::sendMessage(Message* msg, int identity) {
  // if (DEBUG) { Serial.println("sendMessage()"); }
  if (identity != EVERYONE && msg->destination == identity)
    return true;

    Stream* activeComm;
  if (msg->destination == MASTER) {
    // if (DEBUG) { Serial.println("Message is for MASTER"); }
    activeComm = backComm;
  } else {
    // if (DEBUG) { Serial.println("Message is not for MASTER"); }
    activeComm = frontComm;
  }
  if (sendSynAndWaitForAck(activeComm)) {
    activeComm->write(msg->destination);
    activeComm->write(msg->command);
    activeComm->write(msg->arg);
    return true;
  }
  return false;
}

bool ProtocolController::sendSynAndWaitForAck(Stream* comm) {
  int tryCount = 0;
  do {
    comm->write(SYN);
    delay(100);
    if (tryCount++ == 30) {
      return false;
    }
} while (comm->available() == 0 || comm->read() != ACK);
  return true;
}

Stream* ProtocolController::waitForSynAndSendAck() {
  // if (DEBUG) { Serial.println("waitForSynAndSendAck()"); }
  Stream* activeComm;
  bool forward = true;
  do {
    if (forward) {
    //   if (DEBUG) { Serial.println("Opening forward serial port"); }
      activeComm = frontComm;
    } else {
    //   if (DEBUG) { Serial.println("Opening backward serial port"); }
      activeComm = backComm;
    }
    forward = !forward;

    // THIS DELAY MUST BE GREATER THAN THE DELAY BETWEEN SYNS OR ELSE HARMONICS MAY BREAK EVERYTHING
    delay(300);
  } while (activeComm->available() == 0 || activeComm->read() != SYN);

  // if (DEBUG) { Serial.println("Received SYN, now sending ACK"); }
  activeComm->write(ACK);
  // Eat up any extra SYNs that got sent during the delay
  while (activeComm->available() > 0 && activeComm->peek() == SYN) {
    activeComm->read();
  }
  return activeComm;
}

Stream* ProtocolController::getFrontComm() {
    return frontComm;
}
Stream* ProtocolController::getBackComm() {
    return backComm;
}
