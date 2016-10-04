#ifndef UNIT_TEST
#ifndef MESSAGE_PROCESSOR_H_
#define MESSAGE_PROCESSOR_H_

#include "HardwareController.cpp"

class MessageProcessor {
    public:
        MessageProcessor(HardwareController* hardwareController);
        void processMessage(Message* message);

    private:
        void executeCommand(Message* message, int id);

        HardwareController* hardwareController;
};

#endif
#endif
