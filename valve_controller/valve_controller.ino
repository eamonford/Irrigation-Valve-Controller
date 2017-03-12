
#include <AltSoftSerial.h>
#include <avr/io.h>
#include <EEPROM.h>
#include "ProtocolController.h"

#define SOFTWARE_VERSION "001"
#define DEVICE_ID_LEN 1
// TODO: change this to the actual valve control pin
#define VALVE_CONTROL_PIN A3

#define IDENTITY_ADDRESS 32

AltSoftSerial backPort;
ProtocolController* protocolController;

void setup() {
    pinMode(VALVE_CONTROL_PIN, OUTPUT);

  // put your setup code here, to run once:
  Serial.begin(9600);
  backPort.begin(9600);

  protocolController = new ProtocolController(&Serial, &backPort);
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

bool datagramIsForMe(int destination, int id) {
  return (id == 0x00 && destination == FIRST_UNIDENTIFIED) ||
         destination == id || 
         destination == EVERYONE;
}

bool shouldForwardDatagram(int destination, int id) {
  return (destination == EVERYONE || 
         destination == MASTER || 
         destination != id) && !(id == 0x00 && destination == FIRST_UNIDENTIFIED);
}

void executeCommand(Datagram* datagram, int id) {
  switch (datagram->command) {
      case SET_ID:
        if (setId(datagram->arg)) {
          protocolController->sendDatagram(new Datagram(MASTER, 0x01, datagram->arg), id);
        } else {
          protocolController->sendDatagram(new Datagram(MASTER, 0x01, 0x00), id);
        }
        break;
      case OPEN_VALVE:
        digitalWrite(VALVE_CONTROL_PIN, HIGH);
        protocolController->sendDatagram(new Datagram(MASTER, OPEN_VALVE, 0x01), id);
        break;
      case CLOSE_VALVE:
        digitalWrite(VALVE_CONTROL_PIN, LOW);
        protocolController->sendDatagram(new Datagram(MASTER, CLOSE_VALVE, 0x01), id);
        break;
      case IDENTIFY:
          protocolController->sendDatagram(new Datagram(MASTER, 0x01, id), id);
        break;
      default:
//        activePort->println("UNRECOGNIZED COMMAND");
        break;
    }
}

void processDatagram(Datagram* datagram) {
  int id = hasId() ? getId() : EVERYONE;
  if (datagramIsForMe(datagram->destination, id)) {
    executeCommand(datagram, id);
  }
  if (shouldForwardDatagram(datagram->destination, id)) {
    protocolController->sendDatagram(datagram, id);
  }
}

// Won't speak unless spoken to.
void loop() {
  while (true) {
    protocolController->waitForSynAndSendAck();
    int* datagramBytes = protocolController->readBytes(MAX_MSG_LEN);
    Datagram* datagram = Datagram::parse(datagramBytes);
    free(datagramBytes);
    processDatagram(datagram);
    delete datagram;
  }
}
