#ifndef PROTOCOL_CONTROLLER
#define PROTOCOL_CONTROLLER

#include <chain-gang-protocol.h>

class ProtocolController : public CGPInterface {
    private:
        Stream* frontComm;
        Stream* backComm;
    public:
        ProtocolController(Stream* newFrontComm, Stream* newBackComm);
        bool sendDatagram(Datagram* msg, int identity);
        Datagram* getDatagram();
};

#endif

