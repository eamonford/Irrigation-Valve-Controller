#ifndef CGP_INTERFACE_H_
#define CGP_INTERFACE_H_

#include <Arduino.h>
#include "Message.h"

class CGPInterface {
    public:
        virtual bool sendMessage(Message* msg, int identity) = 0;
        virtual bool sendSynAndWaitForAck(Stream* stream) = 0;
        virtual Stream* waitForSynAndSendAck() = 0;
        virtual int* readBytes(Stream* comm, int numBytesToRead) = 0;
};

#endif
