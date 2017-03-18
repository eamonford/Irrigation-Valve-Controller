#include "ProtocolController.h"



void flashLED(int milli) {
  digitalWrite(LED_BUILTIN, HIGH);
  delay(milli);
  digitalWrite(LED_BUILTIN, LOW);
  delay(milli);
}

void holdLEDOn(int milli) {
  digitalWrite(LED_BUILTIN, HIGH);
  delay(milli);
  digitalWrite(LED_BUILTIN, LOW);
}
  
void flashDatagram(Datagram* datagram) {
  for (int i = 0; i < 20; i++) {
      flashLED(50);
  }
  for (int i = 0; i < datagram->destination; i++) {
    flashLED(500);
  }
  delay(1000);
  flashLED(100);
  flashLED(100);
  delay(1000);
  for (int i = 0; i < datagram->command; i++) {
    flashLED(500);
  }
  delay(1000);
  flashLED(100);
  flashLED(100);
  delay(1000);
  for (int i = 0; i < datagram->arg; i++) {
    flashLED(500);
  }
  holdLEDOn(2000);
  delay(2000);
}

ProtocolController::ProtocolController(Stream* newFrontComm, Stream* newBackComm) {
    this->frontComm = newFrontComm;
    this->backComm = newBackComm;
  }

void ProtocolController::sendDatagram(Datagram* datagram, int identity) {
  // Can't send message to self
  if (identity != EVERYONE && datagram->destination == identity)
    return false;
  Stream* comm = datagram->destination == MASTER ? backComm : frontComm;
  CGPInterface::sendDatagram(comm, datagram);
}


Datagram* ProtocolController::getDatagram(int timeout) {
    Stream* allComms[] = {frontComm, backComm};
    Datagram* datagram = CGPInterface::getDatagram(2, allComms, timeout);
    flashDatagram(datagram);
    return datagram;
}

// ====================================================

//Datagram* ProtocolController::getDatagram(int nStreams, Stream** streams, int timeout) {
//  int i = 0;
//  Stream* currStream;
//  do {
//    currStream = streams[i];
//    i = (i + 1) % nStreams;
//  } while (!currStream->available());
//  return getDatagram(currStream, timeout);
//}
//
//Datagram* CGPInterface::getDatagram(Stream* stream, int timeout) {
//  while (true) {
//    char* bytes = (char*)malloc(sizeof(char)*128);
//    int numBytes = readUntil(bytes, stream, END_OF_DATAGRAM, timeout);
//    if (numBytes == 4) {
//      Datagram* newDatagram = Datagram::parse(bytes);
//      if (lastDatagramId == newDatagram->id) {
//        free(bytes);
//        delete newDatagram;
//      } else {
//        free(bytes);
//        lastDatagramId = newDatagram->id;
//        return newDatagram;
//      }
//    } else if (numBytes == 0) {
//      free(bytes);
//      return NULL;
//    } else {
//      free(bytes);
//    }
//  }
//}

