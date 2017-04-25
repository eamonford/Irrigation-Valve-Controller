//#include "ProtocolController.h"
//
//void flashLED(int milli) {
//  digitalWrite(LED_BUILTIN, HIGH);
//  delay(milli);
//  digitalWrite(LED_BUILTIN, LOW);
//  delay(milli);
//}
//
//void holdLEDOn(int milli) {
//  digitalWrite(LED_BUILTIN, HIGH);
//  delay(milli);
//  digitalWrite(LED_BUILTIN, LOW);
//}
//
//void flashByte(int byte) {
//   for (int i = 0; i < 20; i++) {
//      flashLED(50);
//  }
//  for (int i = 0; i < byte; i++) {
//    flashLED(500);
//  }
//  holdLEDOn(2000);
//}
//  
//void flashDatagram(Datagram* datagram) {
//  for (int i = 0; i < 20; i++) {
//      flashLED(50);
//  }
//  for (int i = 0; i < datagram->destination; i++) {
//    flashLED(500);
//  }
//  delay(1000);
//  flashLED(100);
//  flashLED(100);
//  delay(1000);
//  for (int i = 0; i < datagram->command; i++) {
//    flashLED(500);
//  }
//  delay(1000);
//  flashLED(100);
//  flashLED(100);
//  delay(1000);
//  for (int i = 0; i < datagram->arg; i++) {
//    flashLED(500);
//  }
//  holdLEDOn(2000);
//  delay(2000);
//}
//
//ProtocolController::ProtocolController(Stream* newFrontComm, Stream* newBackComm) {
//    this->frontComm = newFrontComm;
//    this->backComm = newBackComm;
//    this->lastDatagramIdFront = 0;
//    this->lastDatagramIdBack = 0;
//}
//
//void ProtocolController::sendDatagram(Datagram* datagram, int identity) {
//  // Can't send message to self
//  if (identity != EVERYONE && datagram->destination == identity)
//    return false;
//  Stream* comm = datagram->destination == MASTER ? backComm : frontComm;
//  CGPInterface::sendDatagram(comm, datagram);
//}
//
//int ProtocolController::readUntil(int bufferLen, int* buffer, Stream* stream, int terminal, long timeout) {
//  int i = 0;
//  long startTime = millis();
//  while (i < bufferLen && (timeout == 0 || millis() - startTime < timeout)) {
//    if (stream->available() > 0) {
//      int newByte = stream->read();
//      if (newByte != terminal) {
//        buffer[i++] = newByte;
//      } else {
////        flashLED(200);
////        flashLED(200);
//        return i;
//      }
//    } else {
////      flashLED(50);
////      flashLED(50);
//    }
//  }
////  holdLEDOn(1000);
//  return 0;
//}
//
///*
// * Guaranteed to block until a valid datagram is received
// */
//Datagram* ProtocolController::getNextUnreadDatagramAndUpdateLastId(int nStreams, Stream** streams, int** lastDatagramIds) {
//  long timeout = 1000;
//  int i = 0;
//  while (true) {
//    Stream* currStream = streams[i];
//    i = (i + 1) % nStreams;
//    if (currStream->available() > 0) {
//      Datagram* datagram = getDatagramOrJunk(currStream, *(lastDatagramIds[i]), timeout);
//      if (datagram != NULL) {
//        lastDatagramIds[i] = datagram->id
//        return datagram;
//      }
//    }
//  }
//}
//
///*
// * If this function ever returns NULL, that means it ate useless data.
// */
//Datagram* ProtocolController::getDatagramOrJunk(Stream* stream, lastDatagramId, long timeout) {
//  int* bytes;
//  while (bytes = (int*)malloc(sizeof(int)*128)) {
//    int numBytes = readUntil(128, bytes, stream, END_OF_DATAGRAM, timeout);
//    // if this happens, then the available data was a false alarm -- it was probably just noise, so return NULL
//    if (numBytes == 0) {
//      free(bytes);
//      return NULL;
//    }
//    if (numBytes >= 4) {
//      // construct a datagram out of the last 4 bytes before the terminal character
//      Datagram* newDatagram = Datagram::parse(bytes + numBytes - 4);
//      if (lastDatagramId != newDatagram->id) {
////        lastDatagramId = newDatagram->id;
//        free(bytes);
//        return newDatagram;
//      }
//      delete newDatagram;
//    }
//    free(bytes);
//  }
//}
//
//Datagram* ProtocolController::getDatagram() {
//    Stream* allComms[] = {frontComm, backComm};
//    int* lastDatagramIds[] = {&lastDatagramIdFront, &lastDatagramIdBack};
//    return getNextUnreadDatagramAndUpdateLastId(2, allComms, lastDatagramIds);
//}
