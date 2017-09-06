#ifndef DEBUG_H_
#define DEBUG_H_

#include <chain-gang-protocol.h>

#define DEBUG true

class Debug {
public:
  static void flashLED(int milli);
  static void holdLEDOn(int milli);
  static void flashDatagram(Datagram* datagram);
};

#endif
