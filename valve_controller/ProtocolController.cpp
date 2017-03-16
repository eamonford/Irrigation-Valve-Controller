#include "ProtocolController.h"

ProtocolController::ProtocolController(Stream* newFrontComm, Stream* newBackComm) {
    this->frontComm = newFrontComm;
    this->backComm = newBackComm;

    // Default to front comm
    this->activeComm = this->frontComm;
}

bool ProtocolController::sendDatagram(Datagram* msg, int identity) {
  if (identity != EVERYONE && msg->destination == identity)
  // Can't send message to self
    return false;
  if (msg->destination == MASTER)
    activeComm = backComm;
  else
    activeComm = frontComm;

  if (sendSynAndWaitForAck(activeComm)) {
    activeComm->write(msg->destination);
    activeComm->write(msg->command);
    activeComm->write(msg->arg);
    return true;
  }
  return false;
}

Stream* ProtocolController::waitForSynAndSendAck() {
  do {
    if (activeComm == frontComm) {
      activeComm = backComm;
    } else {
      activeComm = frontComm;
    }
    delay(30);
  } while (activeComm->available() == 0 || activeComm->read() != SYN);
  activeComm->write(ACK);
  // Eat up any extra SYNs that got sent during the delay
  while (activeComm->available() > 0 && activeComm->peek() == SYN) activeComm->read();

  return activeComm;
}
