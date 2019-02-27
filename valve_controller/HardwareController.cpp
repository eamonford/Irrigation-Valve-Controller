#include "HardwareController.h"
#include <chain-gang-protocol.h>
#include <EEPROM.h>
#include <Arduino.h>

#define IDENTITY_ADDRESS 32
#define SOFTWARE_VERSION "001"

HardwareController::HardwareController(int newValveControlPin) {
    this->valveControlPin = newValveControlPin;
    pinMode(this->valveControlPin, OUTPUT);
}

bool HardwareController::datagramIsForMe(int destination) {
  int id = getIdOrElse(EVERYONE);
  return (id == EVERYONE && destination == FIRST_UNIDENTIFIED) ||
         destination == id ||
         destination == EVERYONE;
}

bool HardwareController::hasId() {
  return ((int)EEPROM.read(IDENTITY_ADDRESS + 1) == SOFTWARE_VERSION[0] &&
          (int)EEPROM.read(IDENTITY_ADDRESS + 2) == SOFTWARE_VERSION[1] &&
          (int)EEPROM.read(IDENTITY_ADDRESS + 3) == SOFTWARE_VERSION[2]);
}

bool HardwareController::setId(int id) {
  EEPROM.write(IDENTITY_ADDRESS, id);
  EEPROM.write(IDENTITY_ADDRESS + 1, SOFTWARE_VERSION[0]);
  EEPROM.write(IDENTITY_ADDRESS + 2, SOFTWARE_VERSION[1]);
  EEPROM.write(IDENTITY_ADDRESS + 3, SOFTWARE_VERSION[2]);

  return getIdOrElse(EVERYONE) == id;
}

int HardwareController::getIdOrElse(int otherOption) {
  return hasId() ? EEPROM.read(IDENTITY_ADDRESS) : otherOption;
}

void HardwareController::openValve() {
  digitalWrite(valveControlPin, HIGH);
}

void HardwareController::closeValve() {
  digitalWrite(valveControlPin, LOW);
}

