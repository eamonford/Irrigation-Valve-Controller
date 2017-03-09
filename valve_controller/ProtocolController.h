#ifndef PROTOCOL_CONTROLLER
#define PROTOCOL_CONTROLLER

#include <chain-gang-protocol.h>

class ProtocolController : public CGPInterface {
    private:
        Stream* frontComm;
        Stream* backComm;
        Stream* activeComm;
    public:
        ProtocolController(Stream* newFrontComm, Stream* newBackComm);
        bool sendDatagram(Datagram* msg, int identity);
        Stream* waitForSynAndSendAck();
        int* readBytes(int numBytesToRead);
};

#endif

