#ifndef UNIT_TEST
#ifndef MESSAGE_PROCESSOR_H_
#define MESSAGE_PROCESSOR_H_

#include "ProtocolController.cpp"

class MessageProcessor {
    private:
        void executeCommand(Message* message, int id);
        ProtocolController* protocolController;

    public:
        MessageProcessor(ProtocolController* protocolController);
        void processMessage(Message* message);
};

#endif
#endif
