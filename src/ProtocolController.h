#ifndef PROTOCOL_CONTROLLER
#define PROTOCOL_CONTROLLER

#include <Arduino.h>
#include "BoardController.h"
#include "CGPInterface.h"

class ProtocolController : public CGPInterface {
    private:
        BoardController* boardController;
        Stream* frontComm;
        Stream* backComm;

    public:
        ProtocolController(BoardController* boardController, Stream* frontComm, Stream* backComm);

        bool sendMessage(Message* msg, int identity);
        bool sendSynAndWaitForAck();
        void waitForSynAndSendAck();
        int* readBytes(int numBytesToRead);
        Message* waitAndGetMessage();
};

#endif
