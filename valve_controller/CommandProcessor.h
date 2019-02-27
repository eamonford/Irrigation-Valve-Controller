#ifndef COMMAND_PROCESSOR_H_
#define COMMAND_PROCESSOR_H_

#include <chain-gang-protocol.h>
#include "HardwareController.h"

class CommandProcessor : public CGPDelegateInterface {
private:

  HardwareController* hardwareController;
  CGP* cgp;

  void executeCommand(Datagram* datagram, int id);
  
public:
  CommandProcessor(int valveControlPin, Stream* towardMaster, Stream* awayFromMaster);
  void getAndProcessDatagram();
  void processDatagram(Datagram* datagram);
  int getStreamIndexForDatagram(Datagram* datagram);
  bool shouldForwardDatagram(Datagram* datagram);
};

#endif
