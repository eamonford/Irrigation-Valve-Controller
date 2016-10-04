#include "Message.h"
#include "constants.h"

Message::Message(int destination, int command, int arg) {
  this->destination = destination;
  this->command = command;
  this->arg = arg;
}

// static Message* Message::parse(int* intArray) {
//   return new Message(intArray[0], intArray[1], intArray[2]);
// }

bool Message::isForId(int id) {
  return (id == 0x00 && this->destination == FIRST_UNIDENTIFIED) ||
         this->destination == id ||
         this->destination == EVERYONE;
}

bool Message::isForIdOtherThan(int id) {
  return (this->destination == EVERYONE || this->destination == MASTER ||
            this->destination != id) &&
         !(id == 0x00 && this->destination == FIRST_UNIDENTIFIED);
}
