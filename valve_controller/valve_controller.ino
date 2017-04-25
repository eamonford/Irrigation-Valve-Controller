
#include <AltSoftSerial.h>
#include <avr/io.h>
#include <EEPROM.h>
//#include "ProtocolController.h"
#include <chain-gang-protocol.h>
#define SOFTWARE_VERSION "001"
#define DEVICE_ID_LEN 1
// TODO: change this to the actual valve control pin
#define VALVE_CONTROL_PIN A3

#define IDENTITY_ADDRESS 32

bool datagramIsForMe(int destination, int id) {
  return (id == 0x00 && destination == FIRST_UNIDENTIFIED) ||
         destination == id || 
         destination == EVERYONE;
}

bool hasId() {
  return ((int)EEPROM.read(IDENTITY_ADDRESS + 1) == SOFTWARE_VERSION[0] &&
          (int)EEPROM.read(IDENTITY_ADDRESS + 2) == SOFTWARE_VERSION[1] &&
          (int)EEPROM.read(IDENTITY_ADDRESS + 3) == SOFTWARE_VERSION[2]);
}

bool setId(int id) {
  EEPROM.write(IDENTITY_ADDRESS, id);
  EEPROM.write(IDENTITY_ADDRESS + 1, SOFTWARE_VERSION[0]);
  EEPROM.write(IDENTITY_ADDRESS + 2, SOFTWARE_VERSION[1]);
  EEPROM.write(IDENTITY_ADDRESS + 3, SOFTWARE_VERSION[2]);

  return getId() == id;
}

int getId() {
  return EEPROM.read(IDENTITY_ADDRESS);
}

AltSoftSerial backPort;
//ProtocolController* protocolController;
/*
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
}*/

Stream* streams[] = {&Serial, &backPort};
CGP* cgp;

void executeCommand(Datagram* datagram, int id) {
  switch (datagram->command) {
      case SET_ID:
        if (setId(datagram->arg)) {
          cgp->sendDatagram(new Datagram(MASTER, SET_ID, datagram->arg));
        } else {
          cgp->sendDatagram(new Datagram(MASTER, SET_ID, 0x00));
        }
        break;
      case OPEN_VALVE:
        digitalWrite(VALVE_CONTROL_PIN, HIGH);
        cgp->sendDatagram(new Datagram(MASTER, OPEN_VALVE, id)); 
        break;
      case CLOSE_VALVE:
        digitalWrite(VALVE_CONTROL_PIN, LOW);
        cgp->sendDatagram(new Datagram(MASTER, CLOSE_VALVE, id));
        break;
      case IDENTIFY:
          cgp->sendDatagram(new Datagram(MASTER, IDENTIFY, id));
        break;
      default:
//        activePort->println("UNRECOGNIZED COMMAND");
        break;
    }
}


class CGPDelegate : public CGPDelegateInterface {
  public:
  
  void processDatagram(Datagram* datagram) {
    int id = hasId() ? getId() : EVERYONE;
    if (datagramIsForMe(datagram->destination, id)) {
      executeCommand(datagram, id);
    }
  }
  
  int getStreamIndexForDatagram(Datagram* datagram) {
    int identity = hasId() ? getId() : EVERYONE; 
    if (identity != EVERYONE && datagram->destination == identity)
      return -1;
    return datagram->destination == MASTER ? 1 : 0;
  }
  
  bool shouldForwardDatagram(Datagram* datagram) {
    int id = hasId() ? getId() : EVERYONE;
    int destination = datagram->destination;
     return (destination == EVERYONE || 
         destination == MASTER || 
         destination != id) && !(id == 0x00 && destination == FIRST_UNIDENTIFIED);
  }
};

void setup() {
  pinMode(VALVE_CONTROL_PIN, OUTPUT);
  // put your setup code here, to run once:
  Serial.begin(9600);
  backPort.begin(9600);

  cgp = new CGP(2, streams, new CGPDelegate());
//  protocolController = new ProtocolController(&Serial, &backPort);
}

//bool shouldForwardDatagram(int destination, int id) {
//  return (destination == EVERYONE || 
//         destination == MASTER || 
//         destination != id) && !(id == 0x00 && destination == FIRST_UNIDENTIFIED);
//}

//void processDatagram(Datagram* datagram) {
////  holdLEDOn(500);
//  int id = hasId() ? getId() : EVERYONE;
//  if (datagramIsForMe(datagram->destination, id)) {
//    executeCommand(datagram, id);
//  }
//  if (shouldForwardDatagram(datagram->destination, id)) {
//      protocolController->sendDatagram(datagram, id);
////      protocolController->sendDatagram(new Datagram(MASTER, END_OF_CHAIN, id), id);
//  }
//}

// Won't speak unless spoken to.
void loop() {
  while (true) {
//    Datagram* datagram = protocolController->getDatagram();
//    processDatagram(datagram);
    cgp->getAndProcessDatagram();
  }
}
