#ifndef PROTOCOL_CONTROLLER
#define PROTOCOL_CONTROLLER

#include <Arduino.h>
#include "BoardController.h"
#include "CGPInterface.h"

class ProtocolController : public CGPInterface {
    public:
        BoardController* boardController;
        Stream* frontComm;
        Stream* backComm;

        ProtocolController(BoardController* newBoardController, Stream* frontComm, Stream* backComm);

        bool sendMessage(Message* msg, int identity);
        bool sendSynAndWaitForAck(Stream* stream);
        Stream* waitForSynAndSendAck();
        int* readBytes(Stream* comm, int numBytesToRead);
        Message* waitAndGetMessage();
};

#endif
