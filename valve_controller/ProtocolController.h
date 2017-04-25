//#ifndef PROTOCOL_CONTROLLER
//#define PROTOCOL_CONTROLLER
//
//#include <chain-gang-protocol.h>
//
//class ProtocolController : public CGPInterface {
//    private:
//        Stream* frontComm;
//        Stream* backComm;
//        int lastDatagramIdFront;
//        int lastDatagramIdBack;
//        
//    public:
//        ProtocolController(Stream* newFrontComm, Stream* newBackComm);
//        void sendDatagram(Datagram* msg, int identity);
//        Datagram* getDatagram();
//        Datagram* getDatagramOrJunk(Stream* stream, long timeout);
//        Datagram* ProtocolController::getNextUnreadDatagramAndUpdateLastId(int nStreams, Stream** streams, int** lastDatagramIds);
//        int readUntil(int bufferLen, int* buffer, Stream* stream, int terminal, long timeout);
//};
//
//#endif

