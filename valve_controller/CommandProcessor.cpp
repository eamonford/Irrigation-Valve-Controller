#include "CommandProcessor.h"
#include "Debug.h"

CommandProcessor::CommandProcessor(int valveControlPin, Stream* towardMaster, Stream* awayFromMaster) {
  this->hardwareController = new HardwareController(valveControlPin);
  Stream** streams = (Stream**)malloc(sizeof(Stream*)*2);
  streams[0] = towardMaster;
  streams[1] = awayFromMaster;

  this->cgp = new CGP(2, streams, this);
}

void CommandProcessor::executeCommand(Datagram* datagram, int id) {
  switch (datagram->command) {
      case SET_ID:
        if (hardwareController->setId(datagram->arg)) {
          cgp->sendDatagram(new Datagram(MASTER, SET_ID, datagram->arg));
        } else {
          cgp->sendDatagram(new Datagram(MASTER, SET_ID, EVERYONE));
        }
        break;
      case OPEN_VALVE:
        hardwareController->openValve();
        cgp->sendDatagram(new Datagram(MASTER, OPEN_VALVE, id));
        break;
      case CLOSE_VALVE:
        hardwareController->closeValve();
        cgp->sendDatagram(new Datagram(MASTER, CLOSE_VALVE, id));
        break;
      case IDENTIFY:
          cgp->sendDatagram(new Datagram(MASTER, IDENTIFY, id));
        break;
      default:
        break;
   }
}

void CommandProcessor::getAndProcessDatagram() {
  cgp->getAndProcessDatagram();
}

void CommandProcessor::processDatagram(Datagram* datagram) {
  #if DEBUG == true
    Debug::flashDatagram(datagram);
  #endif

  if (hardwareController->datagramIsForMe(datagram->destination)) {
    executeCommand(datagram, hardwareController->getIdOrElse(EVERYONE));
  }
}

int CommandProcessor::getStreamIndexForDatagram(Datagram* datagram) {
  int identity = hardwareController->getIdOrElse(EVERYONE);
  if (identity != EVERYONE && datagram->destination == identity)
    return -1;
  return datagram->destination == MASTER ? 0 : 1;
}

bool CommandProcessor::shouldForwardDatagram(Datagram* datagram) {
  int id = hardwareController->getIdOrElse(EVERYONE);
  int destination = datagram->destination;
   return (destination == EVERYONE ||
       destination == MASTER ||
       destination != id) && !(id == EVERYONE && destination == FIRST_UNIDENTIFIED);
}
