#include <avr/io.h>
#include <EEPROM.h>

#define SOFTWARE_VERSION "001"
#define MAX_MSG_LEN 3
#define FORWARD_RX_PIN A1
#define BACKWARD_RX_PIN A2
#define FORWARD_TX_PIN A3
#define BACKWARD_TX_PIN A4
#define ACK 0x10
#define DEVICE_ID_LEN 1

#define PEER 0x08
#define MYSELF 0x02
#define MASTER 0x04

#define IDENTITY_ADDRESS 32

// Addresses
const int master = 0x03;
const int identity = 0x04;
const int everyone = 0x01;

// Commands
const int SET_ID = 0x05;
const int OPEN_VALVE = 0x06;
const int CLOSE_VALVE = 0x07;
const int IDENTIFY = 0x08;

class Message {
  public:
    int destination;
    int command;
    int arg;
    
    Message(int destination, int command, int arg) {
      this->destination = destination;
      this->command = command;
      this->arg = arg;
    }
    
    static Message* parse(int* intArray) {
      return new Message(intArray[0], intArray[1], intArray[2]);      
    }
};

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
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

void sendMessage(Message* msg) { 
  switch (msg->destination) {
    case identity:
      return;
    case master:
      openBackwardTx();
      break;
    default:
      openForwardTx();
  }
  Serial.write(msg->destination);
  Serial.write(msg->command);
  Serial.write(msg->arg);
}

void openForwardRx() {
  digitalWrite(FORWARD_TX_PIN, HIGH);
  digitalWrite(BACKWARD_TX_PIN, LOW);
}

void openBackwardRx() {
  digitalWrite(FORWARD_TX_PIN, LOW);
  digitalWrite(BACKWARD_TX_PIN, HIGH);
}

void openForwardTx() {
  digitalWrite(FORWARD_RX_PIN, HIGH);
  digitalWrite(BACKWARD_RX_PIN, LOW);
}

void openBackwardTx() {
  digitalWrite(FORWARD_TX_PIN, LOW);
  digitalWrite(BACKWARD_TX_PIN, HIGH);
}


void sendAck() {
  sendMessage(new Message(master, ACK, 0x00));
}

int* readBytes(int numBytesToRead) {
//  int message[numBytesToRead];
  int* message = (int*)malloc(sizeof(int)*numBytesToRead);
  
  int numBytesReceived = 0;
  while (numBytesReceived < numBytesToRead) {
    if (Serial.available() > 0) {
      message[numBytesReceived++] = Serial.read();
    }
  }
  return message;
}

int waitForSynAndLockPort() {
  while (true) {
    openForwardRx();
    delay(500);
    if (Serial.available()) {
      Serial.read();
      return;
    }
    openBackwardRx();
    if (Serial.available()) {
      Serial.read();
      return;
    }
  }
}

int getDestinationGroup(int destination) {
  switch (destination) {
    case everyone:
      return MYSELF | PEER;
    case master:
      return MASTER;
    case identity:
      return MYSELF;
    default:
      return PEER;  
  }
}

void processMessage(Message* message) {
  int destinationGroup = getDestinationGroup(message->destination);
  if (destinationGroup & PEER) {
    // pass it along
    sendMessage(message);
  }
  if (destinationGroup & MASTER) {
    // send it to master
    sendMessage(message);
  }
  if (destinationGroup & MYSELF) {
    switch (message->command) {
      case SET_ID:
        if (setId(message->arg)) {
          sendMessage(new Message(master, 0x01, message->arg));
        } else {
          sendMessage(new Message(master, 0x01, 0x00));
        }
        break;
      case OPEN_VALVE:
        Serial.println("OPEN VALVE");
        break;
      case CLOSE_VALVE:
        Serial.println("CLOSE VALVE");
        break;
      case IDENTIFY:
        if (hasId()) {
          sendMessage(new Message(master, 0x01, getId()));
        } else {
          sendMessage(new Message(master, 0x01, 0x00));
        }
        break;
      default:
        Serial.println("UNRECOGNIZED COMMAND");
    }
  }
}

void loop() {
    waitForSynAndLockPort();
    sendAck();
    int* messageBytes = readBytes(MAX_MSG_LEN);
    Message* message = Message::parse(messageBytes);
    free(messageBytes);
    processMessage(message);
    delete message;
}
