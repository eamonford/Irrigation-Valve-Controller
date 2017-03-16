#include "ProtocolController.h"

ProtocolController::ProtocolController(Stream* newFrontComm, Stream* newBackComm) {
    this->frontComm = newFrontComm;
    this->backComm = newBackComm;
  }

bool ProtocolController::sendDatagram(Datagram* msg, int identity) {
  // Can't send message to self
  if (identity != EVERYONE && msg->destination == identity)
    return false;

  Stream* comm = msg->destination == MASTER ? backComm : frontComm;
  if (sendSynAndWaitForAck(comm)) {
    comm->write(msg->destination);
    comm->write(msg->command);
    comm->write(msg->arg);
    return true;
  }
  return false;
}

Datagram* ProtocolController::getDatagram() {
    Stream* allComms[] = {frontComm, backComm};
    Stream* activeComm = waitForSynAndSendAck(2, allComms);
    int* datagramBytes = readBytes(activeComm, MAX_MSG_LEN);
    Datagram* datagram = Datagram::parse(datagramBytes);
    free(datagramBytes);
    return datagram;
}
