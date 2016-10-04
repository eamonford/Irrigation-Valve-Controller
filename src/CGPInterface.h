#ifndef CGP_INTERFACE_H_
#define CGP_INTERFACE_H_

#include "Message.h"

class CGPInterface {
    public:
        virtual bool sendMessage(Message* msg, int identity) = 0;
        virtual bool sendSynAndWaitForAck() = 0;
        virtual void waitForSynAndSendAck() = 0;
        virtual int* readBytes(int numBytesToRead) = 0;
        virtual Message* waitAndGetMessage() = 0;
        Stream* frontPort;
        Stream* backPort;
};
