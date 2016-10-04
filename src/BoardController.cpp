#include <Arduino.h>
#include <EEPROM.h>
#include "BoardController.h"
#include "constants.h"

BoardController::BoardController(int valvePin) {
    this->valvePin = valvePin;
}

int BoardController::getId() {
  return EEPROM.read(IDENTITY_ADDRESS);
}

bool BoardController::hasId() {
  return ((int)EEPROM.read(IDENTITY_ADDRESS + 1) == SOFTWARE_VERSION[0] &&
          (int)EEPROM.read(IDENTITY_ADDRESS + 2) == SOFTWARE_VERSION[1] &&
          (int)EEPROM.read(IDENTITY_ADDRESS + 3) == SOFTWARE_VERSION[2]);
}

bool BoardController::setId(int id) {
  EEPROM.write(IDENTITY_ADDRESS, id);
  EEPROM.write(IDENTITY_ADDRESS + 1, SOFTWARE_VERSION[0]);
  EEPROM.write(IDENTITY_ADDRESS + 2, SOFTWARE_VERSION[1]);
  EEPROM.write(IDENTITY_ADDRESS + 3, SOFTWARE_VERSION[2]);

  return getId() == id;
}

void BoardController::openValve() {
    digitalWrite(valvePin, HIGH);
}

void BoardController::closeValve() {
    digitalWrite(valvePin, LOW);
}
